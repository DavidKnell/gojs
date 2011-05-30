extern void *initJS(void);
extern void *newJSContext(void *);
extern void *destroyJSContext(void *);
extern void *destroyJSRuntime(void *);
extern void *newJSGlobal(void *);
extern void execJS(void *, void *, char *);
void defineCallback(void *cx, void *, char *, int);
extern void setJSProperty(void *, void *, char *path, char *val, int);
