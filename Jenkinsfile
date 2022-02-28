def projectName = "triqs"
/* which platform to build documentation on */
def documentationPlatform = "ubuntu-clang"
def keepInstall = !env.BRANCH_NAME.startsWith("PR-")

properties([
  disableConcurrentBuilds(),
  buildDiscarder(logRotator(numToKeepStr: '10', daysToKeepStr: '30'))
])

/* map of all builds to run, populated below */
def platforms = [:]

/****************** linux builds (in docker) */
/* Each platform must have a cooresponding Dockerfile.PLATFORM in triqs/packaging */
def dockerPlatforms = ["ubuntu-clang", "ubuntu-gcc"]
/* .each is currently broken in jenkins */
for (int i = 0; i < dockerPlatforms.size(); i++) {
  def platform = dockerPlatforms[i]
  platforms[platform] = { -> node('docker') {
    stage(platform) { timeout(time: 1, unit: 'HOURS') {
      checkout scm
      /* construct a Dockerfile for this base */
      sh """
        ( cat packaging/Dockerfile.${env.STAGE_NAME} ; sed '0,/^FROM /d' Dockerfile.build ) > Dockerfile
      """
      /* build and tag */
      def img = docker.build("flatironinstitute/${projectName}:${env.BRANCH_NAME}-${env.STAGE_NAME}", "--build-arg BUILD_DOC=${platform==documentationPlatform} .")
      if (!keepInstall) {
        sh "docker rmi --no-prune ${img.imageName()}"
      }
    } }
  } }
}

/****************** osx builds (on host) */
def osxPlatforms = [
  ["gcc", ['CC=gcc-11', 'CXX=g++-11', 'FC=gfortran-11']],
  ["clang", ['CC=$BREW/opt/llvm/bin/clang', 'CXX=$BREW/opt/llvm/bin/clang++', 'FC=gfortran-11', 'CXXFLAGS=-I$BREW/opt/llvm/include', 'LDFLAGS=-L$BREW/opt/llvm/lib']]
]
for (int i = 0; i < osxPlatforms.size(); i++) {
  def platformEnv = osxPlatforms[i]
  def platform = platformEnv[0]
  platforms["osx-$platform"] = { -> node('osx && triqs') {
    stage("osx-$platform") { timeout(time: 1, unit: 'HOURS') { ansiColor('xterm') {
      def srcDir = pwd()
      def tmpDir = pwd(tmp:true)
      def buildDir = "$tmpDir/build"
      /* install real branches in a fixed predictable place so apps can find them */
      def installDir = keepInstall ? "${env.HOME}/install/${projectName}/${env.BRANCH_NAME}/${platform}" : "$tmpDir/install"
      def venv = installDir

      checkout scm

      def hdf5 = "${env.BREW}/opt/hdf5@1.10"
      dir(buildDir) { withEnv(platformEnv[1].collect { it.replace('\$BREW', env.BREW) } + [
          "PATH=$venv/bin:${env.BREW}/bin:/usr/bin:/bin:/usr/sbin",
          "HDF5_ROOT=$hdf5",
          "C_INCLUDE_PATH=$hdf5/include:${env.BREW}/include",
          "CPLUS_INCLUDE_PATH=$venv/include:$hdf5/include:${env.BREW}/include",
          "LIBRARY_PATH=$venv/lib:$hdf5/lib:${env.BREW}/lib",
          "LD_LIBRARY_PATH=$hdf5/lib",
          "OMP_NUM_THREADS=2",
          "PYTHONPATH=$installDir/lib/python3.9/site-packages",
          "CMAKE_PREFIX_PATH=$venv/lib/cmake/triqs"]) {
        deleteDir()
        sh "python3 -m venv $venv"
        sh "DYLD_LIBRARY_PATH=\$BREW/lib pip3 install -U -r $srcDir/requirements.txt"
        sh "cmake $srcDir -DCMAKE_INSTALL_PREFIX=$installDir -DBuild_Deps=Always"
        sh "make -j2"
        catchError(buildResult: 'UNSTABLE', stageResult: 'UNSTABLE') { try {
          sh "make test CTEST_OUTPUT_ON_FAILURE=1"
        } catch (exc) {
          archiveArtifacts(artifacts: 'Testing/Temporary/LastTest.log')
          throw exc
        } }
        sh "make install"
      } }
    } } }
  } }
}

/****************** sanitization builds (in docker) */
platforms['sanitize'] = { -> node('docker') {
  stage('sanitize') { timeout(time: 1, unit: 'HOURS') {
    checkout scm
    /* construct a Dockerfile for this base */
    sh """
      ( cat packaging/Dockerfile.ubuntu-clang ; sed '0,/^FROM /d' packaging/Dockerfile.sanitize ) > Dockerfile
    """
    def img = docker.build("flatironinstitute/${projectName}:${env.BRANCH_NAME}-sanitize", ".")
    sh "docker rmi --no-prune ${img.imageName()}"
  } }
} }

/****************** wrap-up */
try {
  parallel platforms
  if (keepInstall) {
    node("docker") {
      stage("publish") { timeout(time: 10, unit: 'MINUTES') {
        def commit = sh(returnStdout: true, script: "git rev-parse HEAD").trim()
        def release = env.BRANCH_NAME == "master" || env.BRANCH_NAME == "unstable" || sh(returnStdout: true, script: "git describe --exact-match HEAD || true").trim()
        def workDir = pwd()
        lock('triqs_publish') {
        dir("$workDir/gh-pages") {
          def subdir = "${projectName}/${env.BRANCH_NAME}"
          git(url: "ssh://git@github.com/TRIQS/TRIQS.github.io.git", branch: "master", credentialsId: "ssh", changelog: false)
          sh "rm -rf ${subdir}"
          docker.image("flatironinstitute/${projectName}:${env.BRANCH_NAME}-${documentationPlatform}").inside() {
            sh "cp -rp \$INSTALL/share/doc/${projectName} ${subdir}"
          }
          sh "git add -A ${subdir}"
          sh """
            git commit --author='Flatiron Jenkins <jenkins@flatironinstitute.org>' --allow-empty -m 'Generated documentation for ${subdir}' -m '${env.BUILD_TAG} ${commit}'
          """
          // note: credentials used above don't work (need JENKINS-28335)
          sh "git push origin master || { git pull --rebase origin master && git push origin master ; }"
        }
        if (release) { dir("$workDir/packaging") { try {
          git(url: "ssh://git@github.com/TRIQS/packaging.git", branch: env.BRANCH_NAME, credentialsId: "ssh", changelog: false)
          sh "echo '160000 commit ${commit}\t${projectName}' | git update-index --index-info"
          sh """
            git commit --author='Flatiron Jenkins <jenkins@flatironinstitute.org>' --allow-empty -m 'Autoupdate ${projectName}' -m '${env.BUILD_TAG}'
          """
          // note: credentials used above don't work (need JENKINS-28335)
          sh "git push origin ${env.BRANCH_NAME} || { git pull --rebase origin ${env.BRANCH_NAME} && git push origin ${env.BRANCH_NAME} ; }"
        } catch (err) {
          echo "Failed to update packaging repo"
        } } }
        }
      } }
    }
  }
} catch (err) {
  /* send email on build failure (declarative pipeline's post section would work better) */
  if (env.BRANCH_NAME != "jenkins") emailext(
    subject: "\$PROJECT_NAME - Build # \$BUILD_NUMBER - FAILED",
    body: """\$PROJECT_NAME - Build # \$BUILD_NUMBER - FAILED

$err

Check console output at \$BUILD_URL to view full results.

Building \$BRANCH_NAME for \$CAUSE
\$JOB_DESCRIPTION

Changes:
\$CHANGES

End of build log:
\${BUILD_LOG,maxLines=60}
    """,
    to: 'oparcollet@flatironinstitute.org, nwentzell@flatironinstitute.org, dsimon@flatironinstitute.org',
    recipientProviders: [
      [$class: 'DevelopersRecipientProvider'],
    ],
    replyTo: '$DEFAULT_REPLYTO'
  )
  throw err
}
