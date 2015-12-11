#include <locale.h>
#include </home/pi/openvg/client/config4cpp/include/config4cpp/Configuration.h>
#include <iostream>
#include <stdio.h>


using namespace config4cpp;
using namespace std;
int main(){

setlocale(LC_ALL, "");

const char * logDir;
int logLevel, timeout;
const char * scope = "foo_srv";
Configuration * cfg = Configuration::create();
try {
//cfg->parse(getenv("FOO_CONFIG"));
cfg->parse("testConfig");
logDir = cfg->lookupString(scope, "log.dir");
logLevel = cfg->lookupInt(scope, "log.level");
timeout = cfg->lookupDurationSeconds(scope, "timeout");
} catch(const ConfigurationException & ex) { 
cout << ex.c_str() << endl;
}
cfg->destroy();

cout<<timeout<<endl;

}