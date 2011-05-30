// C part of gojs implementation
// (c) 3C Ltd 2011

/* the next line works for OSX or replace with your platform from the list above. */
#define XP_UNIX
#include "jsapi.h"
#include "stdio.h"
#include <string.h>

// Define our own stubs
JSBool jstStub(void) {
  return JS_TRUE;
}

JSBool jsfStub(void) {
  return JS_FALSE;
}

void jsvStub(void) {
  return;
}

/* The class of the global object. */
static JSClass global_class = {
    "global", JSCLASS_GLOBAL_FLAGS,
    jstStub, jstStub, jstStub, jstStub,
    jstStub, jstStub, jstStub, jsvStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};

/* The error reporter callback. */
void reportError(JSContext *cx, const char *message, JSErrorReport *report)
{
    fprintf(stderr, "%s:%u:%s\n",
            report->filename ? report->filename : "<no filename>",
            (unsigned int) report->lineno,
            message);
}

void *initJS(void) {
  return JS_NewRuntime(8L * 1024L * 1024L);
}

void *newJSContext(JSRuntime *rt) {
  JSContext *cx;

  // Test for runtime
  if (rt == NULL) return NULL;

  // Create context and set options
  cx = JS_NewContext(rt, 8192);
  if (cx == NULL) return NULL;
  JS_SetOptions(cx, JSOPTION_VAROBJFIX | JSOPTION_JIT | JSOPTION_METHODJIT);
  JS_SetVersion(cx, JSVERSION_LATEST);
  JS_SetErrorReporter(cx, reportError);
  return cx;
}

void destroyJSContext(JSContext *cx) {
    printf("DC\n");
    JS_DestroyContext(cx);
    printf("DC end\n");
}

void destroyJSRuntime(JSRuntime *rt, JSContext *cx) {
    JS_DestroyRuntime(rt);
}


// Tell me what's in an object
void dumpObject(JSContext *cx, JSObject *obj) {
  JSObject *io = JS_NewPropertyIterator(cx, obj);
  jsid id;
  if (io == NULL) {
    printf("NewPropertyIterator failed\n");
    exit(0);
  }
  while (JS_NextProperty(cx, io, &id) == JS_TRUE) {
    printf("IDP: %08x\n", id);
    if (id == JSID_VOID) break;
    jsval jv;
    JS_IdToValue(cx, id, &jv);
    getJSValType(cx, jv);
    break;
  }
}

// Helper function - tells me what type a jsval is
void getJSValType(JSContext *cx, jsval jv) {
  if (JSVAL_IS_NULL(jv)) printf("null\n");
  if (JSVAL_IS_STRING(jv)) printf("string: %s\n", JS_EncodeString(cx, JSVAL_TO_STRING(jv)));
  if (JSVAL_IS_OBJECT(jv)) {
    printf("object\n");
    dumpObject(cx, JSVAL_TO_OBJECT(jv));
  }
  if (JSVAL_IS_NUMBER(jv)) printf("number\n");
}

// Generic callback handler
JSBool _gojs_callback(JSContext *cx, uintN argc, jsval *vp)
{
  JSString *fname;
  JSObject *fargs;
  jsval *argv = JS_ARGV(cx, vp);
  printf("Callback %d args %08x\n", argc);
  getJSValType(cx, argv[0]);
  getJSValType(cx, argv[1]);
  if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "So", &fname, &fargs)) return JS_FALSE;
  printf("Function name: %s\n", JS_EncodeString(cx, fname));
  dumpObject(cx, fargs);
  return JS_TRUE;
}

void *newJSGlobal(JSContext *cx) {
  JSObject *global;

  // Create the global object in a new compartment.
  global = JS_NewCompartmentAndGlobalObject(cx, &global_class, NULL);
  if (global == NULL) return NULL;

  // Populate the global object with the standard globals, like Object and Array.
  if (!JS_InitStandardClasses(cx, global)) return NULL;

  // Add callback handler
  if (!JS_DefineFunction(cx, global, "_gojs_callback", _gojs_callback, 2, 0)) return NULL;
  return global;
}

// Define a callback function with given name and max number of parameters
void defineCallback(JSContext *cx, JSObject *global, char *name, int pmax) {
  if (!JS_DefineFunction(cx, global, name, _gojs_callback, pmax, 0)) {
    //!!
    printf("JS_DefineFunction failed\n");
    exit(0);
  }
}

void execJS(JSContext *cx, JSObject *global, char *prog) {
  jsval rval;
  JSBool ok;
  ok = JS_EvaluateScript(cx, global, prog, strlen(prog), "test", 1, &rval);
  if (ok) {
    printf("Exec OK\n");
    if (JSVAL_IS_STRING(rval)) {
      printf("Got a string: %s\n", JS_EncodeString(cx, JSVAL_TO_STRING(rval)));
    }
  }
}

// Set a property on the JS side.  Path is a.b.c - a, a.b will be created on the
// global object as objects in their own right if needed.
int setJSProperty(JSContext *cx, JSObject *global, char *path, char *val, int ro) {
  // Get and follow path
  char *leaf = strrchr(path, '.');
  char *ppath;
  JSObject *obj = global;
  jsval jv;

  if (leaf == NULL) {
    // Single level - set on global object
    jv = STRING_TO_JSVAL(JS_NewStringCopyN(cx, val, strlen(val)));
    JS_SetProperty(cx, obj, path, &jv);
    return 0;
  }
  // Chop off leafname
  *(leaf++) = 0;
  // Follow down path
  ppath = strtok(path, ".");
  while (ppath) {
    printf("Path: %s\n", path);
    // Got an object here?
    if (JS_GetProperty(cx, obj, path, &jv)) {
      if (jv == JSVAL_VOID) {
        // Create object
        obj = JS_DefineObject(cx, obj, ppath, JS_GET_CLASS(cx, obj), NULL, 0);
      } else {
        if (JSVAL_IS_OBJECT(jv)) {
          obj = JSVAL_TO_OBJECT(jv);
        } else {
          //!! Error - not an object..
          return -1;
        }
      }
    } else {
      //!! Error of some sort
      return -1;
    }
    ppath = strtok(NULL, ".");
  }

  // And set the property
  jv = STRING_TO_JSVAL(JS_NewStringCopyN(cx, val, strlen(val)));
  JS_SetProperty(cx, obj, leaf, &jv);
  return 0;
}
