#include "server/WebServer.hpp"
#include "utils/DateUtils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <algorithm>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#define closesocket close
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
typedef int SOCKET;
#endif

namespace medifind::server {

WebServer::WebServer(int port, const std::string& webRoot,
                     database::DatabaseConnection& db,
                     repositories::UserRepository& userRepo,
                     repositories::MedicineRepository& medRepo,
                     repositories::BatchRepository& batchRepo,
                     repositories::SupplierRepository& supplierRepo,
                     repositories::PurchaseOrderRepository& poRepo,
                     repositories::SaleRepository& saleRepo,
                     repositories::ReturnRepository& returnRepo,
                     repositories::StockMovementRepository& movementRepo,
                     repositories::RestockRequestRepository& restockRepo,
                     repositories::AuditLogRepository& auditRepo,
                     services::AuthService& authService,
                     services::InventoryService& invService,
                     services::PurchasingService& purchService,
                     services::SalesService& salesService,
                     services::RestockService& restockService,
                     services::ReportService& reportService,
                     services::AuditService& auditService)
    : port_(port), webRoot_(webRoot),
      db_(db), userRepo_(userRepo), medRepo_(medRepo), batchRepo_(batchRepo),
      supplierRepo_(supplierRepo), poRepo_(poRepo), saleRepo_(saleRepo),
      returnRepo_(returnRepo), movementRepo_(movementRepo), restockRepo_(restockRepo),
      auditRepo_(auditRepo), authService_(authService), invService_(invService),
      purchService_(purchService), salesService_(salesService), restockService_(restockService),
      reportService_(reportService), auditService_(auditService) {}

WebServer::~WebServer() {
    stop();
}

std::string WebServer::getMimeType(const std::string& path) {
    if (path.rfind(".html") != std::string::npos) return "text/html; charset=utf-8";
    if (path.rfind(".css") != std::string::npos) return "text/css; charset=utf-8";
    if (path.rfind(".js") != std::string::npos) return "application/javascript; charset=utf-8";
    if (path.rfind(".json") != std::string::npos) return "application/json; charset=utf-8";
    if (path.rfind(".svg") != std::string::npos) return "image/svg+xml";
    if (path.rfind(".png") != std::string::npos) return "image/png";
    if (path.rfind(".ico") != std::string::npos) return "image/x-icon";
    return "text/plain; charset=utf-8";
}

void WebServer::openBrowser(const std::string& url) {
#ifdef _WIN32
    std::string cmd = "start " + url;
    system(cmd.c_str());
#elif __APPLE__
    std::string cmd = "open " + url;
    system(cmd.c_str());
#else
    std::string cmd = "xdg-open " + url;
    system(cmd.c_str());
#endif
}

std::string WebServer::handleApiRequest(const std::string& method, const std::string& path, const std::string& /*body*/) {
    std::ostringstream json;

    if (path == "/api/status") {
        json << R"({"status":"online","engine":"C++17 SQLite3 FEFO","version":"1.0.0"})";
    }
    else if (path == "/api/dashboard") {
        std::string today = utils::DateUtils::getCurrentDate();
        invService_.refreshExpiryStatuses(today);

        int activeMeds = static_cast<int>(medRepo_.count());
        int activeBatches = static_cast<int>(batchRepo_.countActiveBatches());
        int totalUnits = invService_.getTotalUnitsInStock();
        double costVal = invService_.getTotalInventoryCostValue();
        double retailVal = invService_.getTotalInventoryRetailValue();
        auto lowStock = invService_.getLowStockMedicines();
        auto expired = invService_.getExpiredBatches(today);

        json << "{"
             << "\"activeMedicines\":" << activeMeds << ","
             << "\"activeBatches\":" << activeBatches << ","
             << "\"totalUnits\":" << totalUnits << ","
             << "\"costVal\":" << std::fixed << std::setprecision(2) << costVal << ","
             << "\"retailVal\":" << std::fixed << std::setprecision(2) << retailVal << ","
             << "\"lowStockCount\":" << lowStock.size() << ","
             << "\"expiredCount\":" << expired.size()
             << "}";
    }
    else if (path == "/api/medicines") {
        auto meds = medRepo_.listAll(true);
        json << "[";
        for (size_t i = 0; i < meds.size(); ++i) {
            const auto& m = meds[i];
            int stock = medRepo_.getTotalStockQuantity(m.id);
            if (i > 0) json << ",";
            json << "{"
                 << "\"id\":" << m.id << ","
                 << "\"name\":\"" << m.name << "\","
                 << "\"brand\":\"" << m.brand << "\","
                 << "\"strength\":\"" << m.strength << "\","
                 << "\"dosageForm\":\"" << m.dosageForm << "\","
                 << "\"category\":\"" << m.category << "\","
                 << "\"barcode\":\"" << m.barcode << "\","
                 << "\"minStockThreshold\":" << m.minStockThreshold << ","
                 << "\"totalStock\":" << stock
                 << "}";
        }
        json << "]";
    }
    else if (path == "/api/batches") {
        auto batches = batchRepo_.listAll();
        json << "[";
        for (size_t i = 0; i < batches.size(); ++i) {
            const auto& b = batches[i];
            if (i > 0) json << ",";
            json << "{"
                 << "\"id\":" << b.id << ","
                 << "\"medicineId\":" << b.medicineId << ","
                 << "\"batchNumber\":\"" << b.batchNumber << "\","
                 << "\"mfgDate\":\"" << b.mfgDate << "\","
                 << "\"expiryDate\":\"" << b.expiryDate << "\","
                 << "\"quantity\":" << b.quantity << ","
                 << "\"purchaseCost\":" << std::fixed << std::setprecision(2) << b.purchaseCost << ","
                 << "\"sellingPrice\":" << std::fixed << std::setprecision(2) << b.sellingPrice << ","
                 << "\"status\":\"" << models::batchStatusToString(b.status) << "\""
                 << "}";
        }
        json << "]";
    }
    else if (path == "/api/reports/valuation") {
        std::string report = reportService_.generateInventoryValuationReport();
        json << "{\"report\":\"" << report << "\"}";
    }
    else if (path == "/api/reports/expiry") {
        std::string report = reportService_.generateExpiryAlertReport(utils::DateUtils::getCurrentDate(), 90);
        json << "{\"report\":\"" << report << "\"}";
    }
    else {
        json << R"({"error":"Endpoint not found"})";
    }

    return json.str();
}

void WebServer::handleClient(uintptr_t clientSocketHandle) {
    SOCKET client = static_cast<SOCKET>(clientSocketHandle);
    char buffer[4096];
    int bytesRead = recv(client, buffer, sizeof(buffer) - 1, 0);

    if (bytesRead <= 0) {
        closesocket(client);
        return;
    }

    buffer[bytesRead] = '\0';
    std::string req(buffer);

    std::istringstream stream(req);
    std::string method, fullPath, protocol;
    stream >> method >> fullPath >> protocol;

    std::string path = fullPath;
    auto qPos = path.find('?');
    if (qPos != std::string::npos) {
        path = path.substr(0, qPos);
    }

    if (path.rfind("/api/", 0) == 0) {
        std::string jsonBody = handleApiRequest(method, path, "");
        std::ostringstream response;
        response << "HTTP/1.1 200 OK\r\n"
                 << "Content-Type: application/json; charset=utf-8\r\n"
                 << "Content-Length: " << jsonBody.size() << "\r\n"
                 << "Access-Control-Allow-Origin: *\r\n"
                 << "Connection: close\r\n\r\n"
                 << jsonBody;

        std::string respStr = response.str();
        send(client, respStr.c_str(), static_cast<int>(respStr.size()), 0);
        closesocket(client);
        return;
    }

    // Static File Serving
    if (path == "/" || path.empty()) {
        path = "/index.html";
    }

    std::string filePath = webRoot_ + path;
    std::ifstream file(filePath, std::ios::binary);

    if (!file.is_open()) {
        std::string notFound = "HTTP/1.1 404 Not Found\r\nContent-Length: 13\r\n\r\n404 Not Found";
        send(client, notFound.c_str(), static_cast<int>(notFound.size()), 0);
        closesocket(client);
        return;
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    std::string content = ss.str();
    std::string mime = getMimeType(filePath);

    std::ostringstream response;
    response << "HTTP/1.1 200 OK\r\n"
             << "Content-Type: " << mime << "\r\n"
             << "Content-Length: " << content.size() << "\r\n"
             << "Cache-Control: no-cache\r\n"
             << "Connection: close\r\n\r\n"
             << content;

    std::string respStr = response.str();
    send(client, respStr.c_str(), static_cast<int>(respStr.size()), 0);
    closesocket(client);
}

void WebServer::start() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock.\n";
        return;
    }
#endif

    SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server == INVALID_SOCKET) {
        std::cerr << "Failed to create socket.\n";
        return;
    }

    int opt = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(static_cast<u_short>(port_));

    if (bind(server, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "Failed to bind to port " << port_ << ".\n";
        closesocket(server);
        return;
    }

    if (listen(server, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Failed to listen on socket.\n";
        closesocket(server);
        return;
    }

    serverSocket_ = static_cast<uintptr_t>(server);
    running_ = true;

    std::string url = "http://localhost:" + std::to_string(port_);
    std::cout << "\n=======================================================\n";
    std::cout << "       MEDIFIND CLINICAL WEB SERVER (C++17)            \n";
    std::cout << "=======================================================\n";
    std::cout << " [+] Server active on: " << url << "\n";
    std::cout << " [+] Serving Web Application from: " << webRoot_ << "\n";
    std::cout << " [+] Launching Web UI in your default browser...\n";
    std::cout << "=======================================================\n";
    std::cout << " Press Ctrl+C in this console to stop the server.\n\n";

    openBrowser(url);

    while (running_) {
        sockaddr_in clientAddr{};
        int clientLen = sizeof(clientAddr);
        SOCKET client = accept(server, reinterpret_cast<sockaddr*>(&clientAddr), &clientLen);

        if (client != INVALID_SOCKET) {
            std::thread([this, client]() {
                handleClient(static_cast<uintptr_t>(client));
            }).detach();
        }
    }

    closesocket(server);
#ifdef _WIN32
    WSACleanup();
#endif
}

void WebServer::stop() {
    running_ = false;
    if (serverSocket_ != 0) {
        closesocket(static_cast<SOCKET>(serverSocket_));
        serverSocket_ = 0;
    }
}

} // namespace medifind::server
