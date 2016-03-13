package config

import (
    "os"
    "bufio"
    "strings"
    logger "config/log"
)

func ReadConfigIni(filePath string) map[string]map[string]string {
    configs := make(map[string]map[string]string)

    file, err := os.Open(filePath)
    if err != nil {
        logger.Error.Fatalln(err)
    }
    defer file.Close()

    var optionsTagTitle string

    fileScanner := bufio.NewScanner(file)

    for fileScanner.Scan() {
        line := fileScanner.Text()

        if strings.HasPrefix(line, "[") {
            optionsTagTitle = setTagTitle(&line)
            configs[optionsTagTitle] = map[string]string{}
        } else if len(line) > 0 && !strings.HasPrefix(line, "#") && strings.Contains(line, "=") {
            option := strings.Split(line, "=")
            configs[optionsTagTitle][option[0]] = option[1]
        } else if len(line) > 0 && !strings.HasPrefix(line, "#") && !strings.Contains(line, "=") {
            logger.Warning.Println(line + " does not have = character") // if line is not well formated
        }
    }

    return configs
}

func setTagTitle(line *string) string {
    tag := *line
    tag = strings.TrimLeft(tag, "[")
    tag = strings.TrimRight(tag, "]")
    return tag
}

