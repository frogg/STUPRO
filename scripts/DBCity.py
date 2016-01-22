from xml.dom import minidom
import psycopg2

try:
    connectToDatabaseString = "dbname='testdb' user='stuproTest' host='localhost' password='quappi'"
    connection = psycopg2.connect(connectToDatabaseString)
    cursor = connection.cursor()
except:
    print "not able to connect to database"


def dropTable(tableName):
    cursor = connection.cursor()
    dropTable = "DROP TABLE CITY"
    cursor.execute(dropTable)


def createCityTable(tableName):
    cursor = connection.cursor()
    createTable = "CREATE TABLE IF NOT EXISTS CITY("  \
     "ID SERIAL PRIMARY KEY ," \
    "NAME           TEXT    NOT NULL," \
    "COUNTRYCODES CHAR(2)," \
    "LATITUDE NUMERIC(9, 6)," \
    "LONGITUDE NUMERIC(9, 6));"
    cursor.execute(createTable)


def loadCityDataFromXML(tableName):
    xmldoc = minidom.parse('./cities.xml')
    itemlist = xmldoc.getElementsByTagName('children')
    #print(len(itemlist))
    for s in itemlist:
        #print(s.attributes['name'].value.encode('utf-8'))
        #print(s.attributes['latitude'].value.encode('utf-8'))
        #print(s.attributes['longitude'].value.encode('utf-8'))
        query =  "INSERT INTO " + tableName +" (NAME,COUNTRYCODES,LATITUDE,LONGITUDE) VALUES (%s, %s, %s,%s);"
        data = (s.attributes['name'].value.encode('utf-8') ,'DE', s.attributes['latitude'].value.encode('utf-8'), s.attributes['longitude'].value.encode('utf-8'))
        cursor.execute(query, data)
        connection.commit()


def tableExists(tableName):
    exists = False
    try:
        cursor = connection.cursor()
        cursor.execute("select exists(select relname from pg_class where relname='" + tableName + "')")
        exists = cursor.fetchone()[0]
        cursor.close()
    except psycopg2.Error as e:
        print e
    return exists


cityTableName = 'CITY'
#dropTable(cityTableName)
if (not tableExists(cityTableName)):
    createCityTable(cityTableName)
    loadCityDataFromXML(cityTableName)
