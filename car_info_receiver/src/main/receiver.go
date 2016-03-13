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
/*
	fmt.Println(iniConfigs["QueueConnection"]["port"])
	for title, options := range iniConfigs {
		fmt.Println("[" + title + "]")

		for key, value := range options {
			fmt.Println("  " + key + "=" + value)
		}
	}
*/
	http.LoadConfigs(&iniConfigs)
	http.HttpReceiver()
}

