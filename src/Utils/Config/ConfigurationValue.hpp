#ifndef KRONOS_CONFIGURATION_VALUE_HPP
#define KRONOS_CONFIGURATION_VALUE_HPP

#include <QString>
#include <QList>

class ConfigurationValue {
public:
    /**
     * Constant to be used if the value's type is undefined.
     */
    const static int TYPE_UNKNOWN = 0;
    /**
     * Constant to be used if the value is of type String.
     */
    const static int TYPE_STRING = 1;
    /**
     * Constant to be used if the value is of type Boolean.
     */
    const static int TYPE_BOOLEAN = 2;
    /**
     * Constant to be used if the value is of type Integer.
     */
    const static int TYPE_INTEGER = 3;
    /**
     * Constant to be used if the value is of type Double.
     */
    const static int TYPE_DOUBLE = 4;

    /**
     * A list that contains human-readable names of all possible value types
     */
    const static QList<QString> TYPE_NAMES;

    /**
     * Convenience method for getting a human-readable type name for a value type
     * @param type Integer denoting the value type, should be one of the constant integers with
     * the prefix `TYPE` defined in this class
     * @return A human-readable name of the type
     */
    const static QString getTypeNameFromInteger(int type);

    /**
     * Get the type of this value.
     * @return Integer denoting the type of this value
     */
    const int getType();

    /**
     * Get a human-readable name of the type of this value.
     * @return Human-readable name of the type of this value
     */
    const QString getTypeName();

    /**
     * Default constructor that will set this value's type to undefined
     */
    ConfigurationValue() : valueType(ConfigurationValue::TYPE_UNKNOWN) { }
    virtual ~ConfigurationValue() { }

    /**
     * Create a new ConfigurationValue holding a QString
     * @param value QString value this instance will hold
     */
    ConfigurationValue(QString value)
            : valueType(ConfigurationValue::TYPE_STRING), stringValue(value) { }
    /**
     * Create a new ConfigurationValue holding a Boolean
     * @param value Boolean value this instance will hold
     */
    ConfigurationValue(bool value)
            : valueType(ConfigurationValue::TYPE_BOOLEAN), booleanValue(value) { }
    /**
     * Create a new ConfigurationValue holding a Integer
     * @param value Integer value this instance will hold
     */
    ConfigurationValue(int value)
            : valueType(ConfigurationValue::TYPE_INTEGER), integerValue(value) { }
    /**
     * Create a new ConfigurationValue holding a Double
     * @param value Double value this instance will hold
     */
    ConfigurationValue(double value)
            : valueType(ConfigurationValue::TYPE_DOUBLE), doubleValue(value) { }

    /**
     * Get the QString value this ConfigurationValue holds
     * @return The value this ConfigurationValue holds
     */
    const QString getStringValue();
    /**
     * Get the Boolean value this ConfigurationValue holds
     * @return The value this ConfigurationValue holds
     */
    const bool getBooleanValue();
    /**
     * Get the Integer value this ConfigurationValue holds
     * @return The value this ConfigurationValue holds
     */
    const int getIntegerValue();
    /**
     * Get the Double value this ConfigurationValue holds
     * @return The value this ConfigurationValue holds
     */
    const double getDoubleValue();
private:
    /**
     * Integer denoting the type this ConfigurationValue is of
     */
    int valueType;

    /**
     * The QString value stored in this ConfigurationValue
     */
    QString stringValue;
    /**
     * The Boolean value stored in this ConfigurationValue
     */
    bool booleanValue;
    /**
     * The Integer value stored in this ConfigurationValue
     */
    int integerValue;
    /**
     * The Double value stored in this ConfigurationValue
     */
    double doubleValue;
};

#endif
