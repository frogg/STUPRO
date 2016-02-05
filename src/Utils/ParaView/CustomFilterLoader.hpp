#ifndef SRC_UTILS_PARAVIEW_CUSTOMFILTERLOADER_HPP_
#define SRC_UTILS_PARAVIEW_CUSTOMFILTERLOADER_HPP_
#include <QString>

/**
 * This class is responsible for loading custom filters
 * added to the plugin by default.
 * Common usage: CustomFilterLoader::loadCustomFilters();
 */
class CustomFilterLoader  {
public:
	/**
	 * Loads all custom filters in the fixed folder
	 * using ParaViews built-in custom filter parser.
	 */
	static void loadCustomFilters();

private:
	/**
	 * Is set to the path where the custom filters
	 * are located.
	 */
	static const QString customFilterPath;
};

#endif