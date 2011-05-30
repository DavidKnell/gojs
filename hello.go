// Test JS

package main

import "gojs"
import "fmt"

func cb() {
  fmt.Println("Callback")
}

func main() {
  jsx := gojs.Init()
  js := gojs.NewContext(jsx)
  gojs.SetProperty(js, "test.property", "hi");
  gojs.DefineCallback(js, "test_callback", cb);
  gojs.Exec(js, "test_callback({filename: 'test.wav', play: false});")
  gojs.DestroyContext(js)
}

