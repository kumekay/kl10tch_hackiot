package main

import (
	"fmt"
	"os"
	"time"
	"log"
	"os/exec"
	MQTT "git.eclipse.org/gitroot/paho/org.eclipse.paho.mqtt.golang.git"
)
var f MQTT.MessageHandler = func(client *MQTT.MqttClient, msg MQTT.Message) {
    fmt.Printf("TOPIC: %s\n", msg.Topic())
    fmt.Printf("MSG: %s\n", msg.Payload())
		cmd := exec.Command("play", "sw_cut.mp3")
		err := cmd.Start()
		if err != nil {
			log.Fatal(err)
		}
		log.Printf("Waiting for command to finish...")
		err = cmd.Wait()
		log.Printf("Command finished with error: %v", err)
  }

func main() {
	    opts := MQTT.NewClientOptions().AddBroker("tcp://iot.eclipse.org:1883").SetClientId("hackiot-button-player")
    opts.SetDefaultPublishHandler(f)

    c := MQTT.NewClient(opts)
    _, err := c.Start()
    if err != nil {
      panic(err)
    }

    filter, _ := MQTT.NewTopicFilter("/hackiot/button0", 0)
    if receipt, err := c.StartSubscription(nil, filter); err != nil {
      fmt.Println(err)
      os.Exit(1)
      } else {
        <-receipt
      }

     time.Sleep(5 * time.Hour)
     c.Disconnect(250)
}
