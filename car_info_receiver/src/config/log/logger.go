package config

import (
    "io"
    "io/ioutil"
    "log"
    "os"
)

var (
    Info    *log.Logger
    Warning *log.Logger
    Error   *log.Logger
)

func init()  {
    // default settings
    SetLog(false, true, true)
}

func SetLog(info bool, warn bool, error bool)  {
    var infoHandle io.Writer
    var warningHandle io.Writer
    var errorHandle io.Writer

    if !info {
        infoHandle = ioutil.Discard
    } else {
        infoHandle = os.Stdout
    }

    if !warn {
        warningHandle = ioutil.Discard
    } else {
        warningHandle = os.Stdout
    }

    if !error {
        errorHandle = ioutil.Discard
    } else {
        errorHandle = os.Stderr
    }

    logingConfig(infoHandle, warningHandle, errorHandle)
}

func logingConfig(infoHandle io.Writer, warningHandle io.Writer, errorHandle io.Writer) {

    Info = log.New(infoHandle, "INFO: ", log.Ldate|log.Ltime|log.Lshortfile)

    Warning = log.New(warningHandle, "WARNING: ", log.Ldate|log.Ltime|log.Lshortfile)

    Error = log.New(errorHandle, "ERROR: ", log.Ldate|log.Ltime|log.Lshortfile)
}

func test() {
    SetLog(true, true, true)

    Info.Println("Special Information")
    Warning.Println("There is something you need to know about")
    Error.Println("Something has failed")
}