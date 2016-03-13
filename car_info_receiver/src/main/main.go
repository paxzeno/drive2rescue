package main

import (
	"runtime"
	"http"
	"config"
)

func main() {
	// now go can use all cores to do stuff.
	runtime.GOMAXPROCS(runtime.NumCPU()-1)

	// load configs
	iniConfigs := config.ReadConfigIni("config.ini")

	// start http server
	http.LoadConfigs(&iniConfigs)
	http.HttpReceiver()
}

