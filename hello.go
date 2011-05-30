// Test JS

package main

import "gojs"
import "fmt"
import "syscall"

var jsx *gojs.JS

func cb() {
  fmt.Println("Callback")
}

// Create context, wait 2 seconds, destroy it again
func test() {
  js := gojs.NewContext(jsx)
  syscall.Sleep(2E9)
  gojs.DestroyContext(js)
}

func main() {
  jsx = gojs.Init()
  for {
	go test()
	syscall.Sleep(1E9)	// 1 sec pause
  }
/*
  js := gojs.NewContext(jsx)
  gojs.SetProperty(js, "test.property", "hi");
  gojs.DefineCallback(js, "test_callback", cb);
  gojs.Exec(js, "test_callback({filename: 'test.wav', play: false});")
  gojs.DestroyContext(js)
*/
}

