#include "app.h"

int main() {
	App* app = App::create_app(500, 500);

	while (!app->should_close())
		app->update();

	delete app;
}