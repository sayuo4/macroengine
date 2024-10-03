#include "core/application.hpp"

#include <stdexcept>

namespace macroengine {
	std::shared_ptr<App> App::inst;

	App::App() {}

	App &App::init() {
		if (inst) {
			throw std::runtime_error("Can't initialize macroengine application more than once\n");
		}

		inst = std::shared_ptr<App>{new App{}};
		return *inst;
	}

	App &App::get() {
		if (!inst) {
			throw std::runtime_error("Can't get macroengine application without initializing it\n");
		}

		return *inst;
	}

	App &App::addStartupSystem(std::function<void()> system) {
		startupSystems.push_back(system);

		return *this;
	}

	App &App::addStartupSystem(std::function<void(Registry&)> system) {
		startupSystems.push_back([this, system]() {
			system(registry);
		});

		return *this;
	}

	App &App::addSystem(std::function<void()> system, Update updateType) {
		auto &systemsVec = updateType == Update::Frame ? frameUpdateSystems : fixedUpdateSystems;

		systemsVec.push_back(system);

		return *this;
	}

	App &App::addSystem(std::function<void(Registry&)> system, Update updateType) {
		auto &systemsVec = updateType == Update::Frame ? frameUpdateSystems : fixedUpdateSystems;

		systemsVec.push_back([this, system]() {
			system(registry);
		});

		return *this;
	}
}
