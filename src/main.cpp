#include "crow.h"
#include "crow/middlewares/cookie_parser.h"

#include "auth/route/auth_routes.h"
#include "product/route/product_routes.h"
#include "order/route/order_routes.h"


int main()
{
    crow::App<crow::CookieParser> app;

    setupAuthRoutes(app);
    setProductRoutes(app);
    setupOrderRoutes(app);

    app.port(18080).multithreaded().run();

    return 0;
}