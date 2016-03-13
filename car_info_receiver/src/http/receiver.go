package http

import (
    "net/http"
    "strings"
    "regexp"
    logger "config/log"
    "rabbitmq"
)

type httpConfigs struct  {
    successResponse []byte
    failResponse []byte
    httpPort string
    urlPath string
    inputRegexValidation string
    parallelSendToQueue bool
}

var configs httpConfigs

func init() {
    // default values for configurations
    configs.successResponse = []byte("success")
    configs.failResponse = []byte("fail")
    configs.httpPort = "8080"
    configs.urlPath = "/car_details/"
    configs.inputRegexValidation = `(\w+),(\d+.\d+)?,(\d+.\d+)?,(\d+)?,(\d+)?$`
    configs.parallelSendToQueue = false
}

func LoadConfigs(iniConfigs *(map[string]map[string]string)) {
    auxIniConfigs := *iniConfigs
    var info bool = false
    var warn bool = false
    var err bool = false

    // RABBITMQ configs
    rabbitmq.LoadConfigs(&auxIniConfigs)

    // HTTP
    if auxIniConfigs["Http"]["success_response"] != "" {
        configs.successResponse = []byte(auxIniConfigs["Http"]["success_response"])
    }

    if auxIniConfigs["Http"]["fail_response"] != "" {
        configs.failResponse = []byte(auxIniConfigs["Http"]["fail_response"])
    }

    if auxIniConfigs["Http"]["Http_port"] != "" {
        configs.httpPort = auxIniConfigs["Http"]["Http_port"]
    }

    if auxIniConfigs["Http"]["url_path"] != "" {
        configs.urlPath = auxIniConfigs["Http"]["url_path"]
    }

    if auxIniConfigs["Http"]["input_regex_validation"] != "" {
        configs.inputRegexValidation = auxIniConfigs["Http"]["input_regex_validation"]
    }

    if auxIniConfigs["Http"]["parallel_send_to_queue"] == "true" {
        configs.parallelSendToQueue = true
    }

    // LOG
    if auxIniConfigs["Log"]["info"] == "true" { info = true }

    if auxIniConfigs["Log"]["warning"] == "true" { warn = true }

    if auxIniConfigs["Log"]["error"] == "true" { err = true }

    logger.SetLog(info, warn, err)
}

func HttpReceiver() {
    http.HandleFunc(configs.urlPath, carInfoCollector)
    http.ListenAndServe(":" + configs.httpPort, nil)
}

func carInfoCollector(w http.ResponseWriter, r *http.Request)  {
    w.Header().Set("Content-Type", "text/html")
    urlPath := r.URL.Path

    data := getData(&urlPath)

    if !validateInput(&data) {
        logger.Warning.Println("Invalid data in request: %s", urlPath)
        w.Write(configs.failResponse)
        return
    }

    if (configs.parallelSendToQueue) {
        go rabbitmq.Send(data)
    } else {
        rabbitmq.Send(data)
    }

    logger.Info.Println("Request proccessed: %s", urlPath)
    w.Write(configs.successResponse)
}

// check if data respects regex pattern
func validateInput(data *string) bool {
    if regexp.MustCompile(configs.inputRegexValidation).MatchString(*data) {
        return true
    }

    return false
}

// remove url path from url: /car_details/ASDFASDF,34.5345,2323.53452323,2323,34 => ASDFASDF,34.5345,2323.53452323,2323,34
func getData(urlPath *string) string {
    path := *urlPath
    lastSpliter := strings.LastIndex(path, "/")
    if lastSpliter != 1 {
        return path[lastSpliter+1: len(path)-1+1]
    }

    return path
}