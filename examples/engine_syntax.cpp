// This file contains the syntax I aim to implement for the engine
#include <macroengine.hpp>

using namespace macroengine;

struct MyComponent : Component {

};

struct MyResource : Resource {

};

struct MyModule : Module {
	void setup(Engine &engine) {

	}
};

using MyPreset = Preset<C1, C2, C3>;

void mySystem() {
	Registry &registry = ECS::registry();
	registry.create()
		.insert(C1{params})
		.insert(C2{params})

		.insert(MyPreset{}, {params}, {params}, {params});
		// or
		.insert(MyPreset{})
		.set(C1{params})
		.set(C2{params})
		.set(C3{params});

	Query &query = ECS::query<C1, C2>().without<C3, C4>();

	for (auto &[c1, c2] : query) {

	}
}

int main() {
	Engine::init()
		.setWindowSize(1280, 720)
		.disableVsync()
		.setFixedUpdateRate(120)
		.setBackgroundColor(Color::DARKGRAY)
		.addSystem(mySystem, Update::Start)
		.addModule(MyModule)
		.addResource(MyResource{});
		.run();
}
