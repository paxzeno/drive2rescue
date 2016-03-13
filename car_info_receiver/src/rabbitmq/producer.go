package rabbitmq

import (
    "github.com/streadway/amqp"
    logger "config/log"
    "config"
)

type rabbitmqConfigs struct {
    queueName string
    amqpUrl string
}

var configs rabbitmqConfigs

func ini()  {
    // default values
    configs.amqpUrl = "amqp://guest:guest@localhost:5672/"
    configs.queueName = ""
}

func LoadConfigs(iniConfigs *(map[string]map[string]string)) {
    auxIniConfigs := *iniConfigs
    var info bool = false
    var warn bool = false
    var err bool = false

    var host string = ""
    var port string = ""
    var user string = ""
    var pass string = ""

    // QueueConnection
    if auxIniConfigs["QueueConnection"]["queue_name"] != "" {
        configs.queueName = auxIniConfigs["QueueConnection"]["queue_name"]
    }

    if auxIniConfigs["QueueConnection"]["host"] != "" {
        host = auxIniConfigs["QueueConnection"]["host"]
    }

    if auxIniConfigs["QueueConnection"]["port"] != "" {
        port = auxIniConfigs["QueueConnection"]["port"]
    }

    if auxIniConfigs["QueueConnection"]["user"] != "" {
        user = auxIniConfigs["QueueConnection"]["user"]
    }

    if auxIniConfigs["QueueConnection"]["pass"] != "" {
        pass = auxIniConfigs["QueueConnection"]["pass"]
    }

    // amqp://guest:guest@localhost:5672/
    configs.amqpUrl = "amqp://" + user + ":" + pass + "@" + host + ":" + port + "/"

    // LOG
    if auxIniConfigs["Log"]["info"] == "true" { info = true }

    if auxIniConfigs["Log"]["warning"] == "true" { warn = true }

    if auxIniConfigs["Log"]["error"] == "true" { err = true }

    logger.SetLog(info, warn, err)
}

func failOnError(err *error, msg string) bool {
    if *err != nil {
        logger.Error.Println("%s: %s", msg, *err)
        return true
    }
    return false
}

func Send(data string) {
    var exitOnError bool

    conn, err := amqp.Dial(configs.amqpUrl)
    exitOnError = failOnError(&err, "Failed to connect to RabbitMQ")
    if exitOnError {
        if conn != nil { conn.Close() }
        return
    }
    defer conn.Close()

    ch, err := conn.Channel()
    exitOnError = failOnError(&err, "Failed to declare a queue")
    if exitOnError {
        if ch != nil { ch.Close() }
        return
    }
    defer ch.Close()

    q, err := ch.QueueDeclare(
        configs.queueName,  // queue name
        true,               // durable
        false,              // delete when unused
        false,              // exclusive
        false,              // no-wait
        nil,                // arguments
    )
    if failOnError(&err, "Failed to declare a queue") {return } // exist on error

    err = ch.Publish("",    // exchange
        q.Name,             // routing key
        false,              // mandatory
        false,
        amqp.Publishing{
            //DeliveryMode: amqp.Persistent,
            ContentType:  "text/plain",
            Body:         []byte(data),
        })
    if !failOnError(&err, "Failed to publish a message") {
        logger.Info.Println("data published: " + data + " ")
    }
}


func test() {
    logger.SetLog(true, true, true)

    // load configs
    iniConfigs := config.ReadConfigIni("config.ini")

    LoadConfigs(&iniConfigs)

    Send("hello world")
}

