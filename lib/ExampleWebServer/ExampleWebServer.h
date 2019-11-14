#include <ESPAsyncWebServer.h>
#include <RichHttpServer.h>
#include <functional>

#pragma once

using RichHttpConfig = RichHttp::Generics::Configs::AsyncWebServer;
using RequestContext = RichHttpConfig::RequestContextType;

class ExampleWebServer {
public:
  ExampleWebServer();
  ~ExampleWebServer();

  void begin();

private:
  RichHttpServer<RichHttpConfig> server;

  void handleNoOp(RequestContext& request);

  // General info routes
  void handleGetSystem(RequestContext& request);
  void handlePostSystem(RequestContext& request);

  // Misc helpers
  void handleUpdateFile(ArUploadHandlerFunction* request, const char* filename);
  void handleServeFile(
    const char* filename,
    const char* contentType,
    const char* defaultText,
    RequestContext& request
  );
  void handleServeGzip_P(
    const char* contentType,
    const uint8_t* text,
    size_t length,
    RequestContext& request
  );
  void _handleServeGzip_P(
    const char* contentType,
    const uint8_t* text,
    size_t length,
    AsyncWebServerRequest* request
  );
  bool serveFile(const char* file, const char* contentType, RequestContext& request);
};