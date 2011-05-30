// Go-Spidermonkey interface library
// (c) 3C Ltd 2011

package gojs

// #include "gojs.h"
import "C"
import "unsafe"
import "fmt"

type JS struct {
  rt, cx, gl unsafe.Pointer
  callback map[string] func()
}

func Init() (*JS) {
  js := new(JS)
  js.rt = C.initJS()
  return js
}

func NewContext(js *JS) (newjs *JS) {
  newjs = new(JS)
  newjs.rt = js.rt
  newjs.cx = C.newJSContext(newjs.rt)
  newjs.gl = C.newJSGlobal(newjs.cx)
  newjs.callback = make(map[string] func())
  fmt.Printf("New context: %v\n", newjs)
  return newjs
}

func DestroyContext(js *JS) {
  C.destroyJSContext(js.cx)
//  C.destroyJSRuntime(js.rt)
}

func Exec(js *JS, prog string) {
  // Build preamble - we create a set of functions which call our
  // callback function with the name of the original function and
  // its arguments as parameters
  preamble := ""
  for key, _ := range(js.callback) {
    preamble += "function " + key + "() { return _gojs_callback('" + key + "', JSON.stringify(arguments)); }\n"
  }
  fmt.Println("Executing: \n" + preamble + prog)

  C.execJS(js.cx, js.gl, C.CString(preamble + prog))
}

// Define a callback - remember the name in our context,
// and define so that it's usable from JS.
func DefineCallback(js *JS, name string, fn func()) {
  js.callback[name] = fn
}

func SetProperty(js *JS, name, value string) {
  C.setJSProperty(js.cx, js.gl, C.CString(name), C.CString(value), 0);
}
