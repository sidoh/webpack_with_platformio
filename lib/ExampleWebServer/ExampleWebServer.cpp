#include <ExampleWebServer.h>

// auto-generated. will be included from dist
#include <web_assets.h>

#include <Update.h>

static const char TEXT_HTML[] = "text/html";
static const char TEXT_PLAIN[] = "text/plain";
static const char APPLICATION_JSON[] = "application/json";
static const AuthProvider authProvider;

using namespace std::placeholders;

ExampleWebServer::ExampleWebServer()
  : server(RichHttpServer<RichHttpConfig>(80, authProvider))
{ }

ExampleWebServer::~ExampleWebServer() {
  server.reset();
}

void ExampleWebServer::begin() {
  // Register auto-generated static assets
  for (auto it = WEB_ASSET_CONTENTS.begin(); it != WEB_ASSET_CONTENTS.end(); ++it) {
    const char* filename = it->first;
    const uint8_t* contents = it->second;
    const size_t length = WEB_ASSET_LENGTHS.at(filename);
    const char* contentType = WEB_ASSET_CONTENT_TYPES.at(filename);

    server
      .buildHandler(filename)
      .on(HTTP_GET, std::bind(&ExampleWebServer::handleServeGzip_P, this, contentType, contents, length, _1));
  }

  server
    .buildHandler("/api/v1/system")
    .on(HTTP_GET, std::bind(&ExampleWebServer::handleGetSystem, this, _1))
    .on(HTTP_POST, std::bind(&ExampleWebServer::handlePostSystem, this, _1));

  server
    .buildHandler("/firmware")
    .handleOTA();

  // Redirect anything prefixed with /app to index.html
  server.onNotFound([this](AsyncWebServerRequest *request) {
    if (request->url() == "/" || request->url().startsWith("/app")) {
      _handleServeGzip_P(TEXT_HTML, INDEX_HTML_GZ, INDEX_HTML_GZ_LENGTH, request);
    } else {
      request->send(404);
    }
  });

  server.clearBuilders();
  server.begin();
}

void ExampleWebServer::handlePostSystem(RequestContext& request) {
  JsonObject body = request.getJsonBody().as<JsonObject>();
  JsonVariant command = body[F("command")];

  if (command.isNull()) {
    request.response.json[F("error")] = F("Command not specified");
    request.response.setCode(400);
    return;
  }

  String strCommand = command.as<String>();

  if (strCommand.equalsIgnoreCase("reboot")) {
    ESP.restart();
    request.response.json[F("success")] = true;
  } else {
    request.response.json[F("error")] = F("Unhandled command");
    request.response.setCode(400);
  }
}

void ExampleWebServer::handleGetSystem(RequestContext& request) {
  // Measure before allocating buffers
  uint32_t freeHeap = ESP.getFreeHeap();

  request.response.json["version"] = "1.0.0";
  request.response.json["variant"] = "esp32";
  request.response.json["free_heap"] = freeHeap;
  request.response.json["sdk_version"] = ESP.getSdkVersion();
}

void ExampleWebServer::handleServeGzip_P(
  const char* contentType,
  const uint8_t* text,
  size_t length,
  RequestContext& request
) {
  _handleServeGzip_P(contentType, text, length, request.rawRequest);
}

void ExampleWebServer::_handleServeGzip_P(
  const char* contentType,
  const uint8_t* text,
  size_t length,
  AsyncWebServerRequest* request
) {
  AsyncWebServerResponse* response = request->beginResponse_P(200, contentType, text, length);
  response->addHeader("Content-Encoding", "gzip");
  request->send(response);
}