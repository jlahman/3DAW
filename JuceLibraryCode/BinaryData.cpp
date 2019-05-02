/* ==================================== JUCER_BINARY_RESOURCE ====================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

namespace BinaryData
{

//================== test_input.txt ==================
static const unsigned char temp_binary_data_0[] =
"import -f ../data/test.wav\n"
"import -f ../data/narrator.ogg\n"
"import -f ../data/narrator2.ogg\n"
"import -f ../data/souldfire.wav\n"
"import -f ../data/Bee.mp3\n"
"\n"
"add -s Waterfall 0\n"
"add -s Narrator 1\n"
"add -s Narrator2 2\n"
"add -s Bonfire 3\n"
"\n"
"select -s Waterfall\n"
"select -k 0\n"
"set -k -p scale 0.3\n"
"set -k -p theta -40\n"
"\n"
"select -s Bonfire\n"
"select -k 0\n"
"set -k -p scale 0.6\n"
"set -k -p theta 20\n"
"\n"
"select -s Narrator\n"
"select -k 0\n"
"set -s -p start_time 5\n"
"set -k -p scale 0.5\n"
"set -k -p theta -100\n"
"add -k 8.5\n"
"select -k 1\n"
"set -k -p scale 0.5\n"
"set -k -p theta 50\n"
"set -k -p visible false\n"
"\n"
"\n"
"select -s Narrator2\n"
"select -k 0\n"
"set -s -p start_time 5.5\n"
"set -k -p scale 0.5\n"
"set -k -p visible false\n"
"\n"
"\n"
"\n";

const char* test_input_txt = (const char*) temp_binary_data_0;


const char* getNamedResource (const char*, int&) throw();
const char* getNamedResource (const char* resourceNameUTF8, int& numBytes) throw()
{
    unsigned int hash = 0;
    if (resourceNameUTF8 != 0)
        while (*resourceNameUTF8 != 0)
            hash = 31 * hash + (unsigned int) *resourceNameUTF8++;

    switch (hash)
    {
        case 0x6ba3850e:  numBytes = 653; return test_input_txt;
        default: break;
    }

    numBytes = 0;
    return 0;
}

const char* namedResourceList[] =
{
    "test_input_txt"
};

}
