Set-PSDebug -Trace 1

function CheckLastExitCode {
    param ([int[]]$SuccessCodes = @(0), [scriptblock]$CleanupScript=$null)

    Push-AppveyorArtifact "$LogFile"

    if ($SuccessCodes -notcontains $LastExitCode) {
        if ($CleanupScript) {
            "Executing cleanup script: $CleanupScript"
            &$CleanupScript
        }
        $msg = @"
EXE RETURNED EXIT CODE $LastExitCode
CALLSTACK:$(Get-PSCallStack | Out-String)
"@
        throw $msg
    }
}

cd  C:\projects\libossia
git submodule update --init --recursive

if ( $env:APPVEYOR_BUILD_TYPE -eq "max" ){
  appveyor DownloadFile https://cycling74.s3.amazonaws.com/download/max-sdk-7.3.3.zip
  7z x max-sdk-7.3.3.zip -y
}

mkdir build
cd build

if ( $env:APPVEYOR_BUILD_TYPE -eq "testing" ){
  if ( Test-Path ${env:QTDIR}\bin\ ) {
    set $env:PATH=${env:QTDIR}\bin;${env:PATH};
  }

  $LogFile = "${env:APPVEYOR_BUILD_FOLDER}\${env:APPVEYOR_BUILD_TYPE}-${env:configuration}.log"
  if ( $env:configuration -eq "Release" ){
    cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release -DOSSIA_PD=0 -DOSSIA_CI=1 -DOSSIA_TESTING=1 -DBOOST_ROOT="${env:BOOST_ROOT}" -DCMAKE_PREFIX_PATH="${env:QTDIR}\lib\cmake\Qt5" c:\projects\libossia > $LogFile
  } else {
    cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Debug   -DOSSIA_PD=0 -DOSSIA_CI=1 -DOSSIA_TESTING=1 -DBOOST_ROOT="${env:BOOST_ROOT}" -DCMAKE_PREFIX_PATH="${env:QTDIR}\lib\cmake\Qt5" c:\projects\libossia > $LogFile
  }
  CheckLastExitCode

} elseif ( $env:APPVEYOR_BUILD_TYPE -eq "max" ){
  $LogFile = c:\projects\libossia\configure-max.log
  cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release -DOSSIA_MAX=1 -DMAXSDK_MAINPATH="${env:APPVEYOR_BUILD_FOLDER}\max-sdk-7.3.3\source" -DOSSIA_PD=0 -DCMAKE_INSTALL_PREFIX="${env:APPVEYOR_BUILD_FOLDER}/install" -DOSSIA_STATIC=1 -DOSSIA_QT=0 -DOSSIA_NO_QT=1 -DOSSIA_EXAMPLES=0 -DOSSIA_CI=1 -DOSSIA_TESTING=0 -DBOOST_ROOT="${env:BOOST_ROOT}" c:\projects\libossia > $LogFile
  CheckLastExitCode

  # now configure 32 bit version
  cd ..
  mkdir build-32bit
  cd build-32bit

  $LogFile = c:\projects\libossia\configure-max-32bit.log
  cmake -G "Visual Studio 15 2017" -DCMAKE_BUILD_TYPE=Release -DOSSIA_MAX=1 -DMAXSDK_MAINPATH="${env:APPVEYOR_BUILD_FOLDER}\max-sdk-7.3.3\source" -DOSSIA_PD=0 -DCMAKE_INSTALL_PREFIX="${env:APPVEYOR_BUILD_FOLDER}/install" -DOSSIA_STATIC=1 -DOSSIA_QT=0 -DOSSIA_NO_QT=1 -DOSSIA_EXAMPLES=0 -DOSSIA_CI=1 -DOSSIA_TESTING=0 -DBOOST_ROOT="${env:BOOST_ROOT}" c:\projects\libossia > $LogFile
  CheckLastExitCode

} elseif ( $env:APPVEYOR_BUILD_TYPE -eq "pd" ){
  $LogFile = c:\projects\libossia\configure-pd.log
  cmake -G "Visual Studio 15 2017" -DCMAKE_BUILD_TYPE=Release -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="${env:APPVEYOR_BUILD_FOLDER}/install" -DOSSIA_STATIC=1 -DOSSIA_QT=0 -DOSSIA_NO_QT=1 -DOSSIA_EXAMPLES=0 -DOSSIA_CI=1 -DOSSIA_TESTING=0 -DBOOST_ROOT="${env:BOOST_ROOT}" c:\projects\libossia > $LogFile
  CheckLastExitCode
}