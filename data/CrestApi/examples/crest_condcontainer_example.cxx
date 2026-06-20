#include <nlohmann/json.hpp>
#include <string>

#include "CrestApi/CrestCondException.h"
#include "CrestContainer.h"
using namespace Crest;
using namespace std;

int main(int argc, char *argv[]) {
  cout << "Test 1 started" << endl;
  CrestContainer cond;
  cond.addColumn("a", Crest::TypeId::UInt16);
  cond.addColumn("b", Crest::TypeId::Bool);
  cond.addColumn("c", Crest::TypeId::Float);
  cond.addColumn("d", Crest::TypeId::Double);
  cond.addColumn("name", Crest::TypeId::String255);
  try {
    cond.addRecord("a", 1, 1);
    cond.addRecord("b", 1, true);
    cond.addRecord("c", 1, 1.0);
    cond.addRecord("name", 1, "test");
    cond.addRecord("d", 1, 2.0);
    cond.addData("1");

    cond.addRecord("a", 1, 2);
    cond.addRecord("b", 1, false);
    cond.addRecord("c", 1, 5.9);
    cond.addRecord("name", 1, "test2");
    cond.addRecord("d", 1, 8.9999);
    cond.addData("2");
    std::cout << "Column index for a is " << cond.getColumnIndex("a")
              << std::endl;
    std::string channelKey = "1";
    nlohmann::json p = cond.getPayloadChannel(channelKey);
    std::cout << "Payload for channel 1 is " << p.dump() << std::endl;

    cond.addIov(1000);
  } catch (CrestBaseException &ex) {
    cout << ex.what() << endl;
  }
  try {
    cout << cond.getJsonPayload() << endl;
    cout << cond.getJsonIovData() << endl;
  } catch (CrestBaseException &ex) {
    cout << ex.what() << endl;
  }
  cout << "Test 1 finished" << endl;

  cout << "Test 2 started" << endl;
  CrestContainer condvp;
  condvp.addColumn("a", Crest::TypeId::UInt16);
  condvp.addColumn("b", Crest::TypeId::Bool);
  condvp.addColumn("c", Crest::TypeId::Float);
  condvp.addColumn("name", Crest::TypeId::String255);
  condvp.setVectorPayload(true);
  try {
    condvp.addRecord("a", 1, 1);
    condvp.addRecord("b", 1, true);
    condvp.addRecord("c", 1, 1.0);
    condvp.addRecord("name", 1, "test");
    condvp.putRow2Vector();
    condvp.addRecord("a", 1, 2);
    condvp.addRecord("b", 1, false);
    condvp.addRecord("c", 1, 2.0);
    condvp.addRecord("name", 1, "test2");
    condvp.putRow2Vector();
    condvp.addData("1");
    std::cout << "Data added...now create an iov" << std::endl;
    condvp.addIov(1000);
  } catch (CrestBaseException &ex) {
    cout << ex.what() << endl;
  }
  try {
    cout << condvp.getJsonIovData() << endl;
  } catch (CrestBaseException &ex) {
    cout << ex.what() << endl;
  }

  cout << "Test 2 finished" << endl;

  // Now add channels
  cout << "Test 3 started" << endl;
  CrestContainer condch;
  condch.addColumn("a", Crest::TypeId::UInt16);
  condch.addColumn("b", Crest::TypeId::Bool);
  condch.addColumn("c", Crest::TypeId::Float);
  condch.addChannel("1", "Channel 1");
  condch.addChannel("2", "Channel 2");
  try {
    condch.addRecord("a", 1, 1);
    condch.addRecord("b", 1, true);
    condch.addRecord("c", 1, 1.0);
    condch.addData("1");
    condch.addRecord("a", 1, 2);
    condch.addRecord("b", 1, false);
    condch.addRecord("c", 1, 2.0);
    condch.addData("2");
    std::cout << "Column index for a is " << condch.getColumnIndex("a")
              << std::endl;
    std::string channelKey = "1";
    nlohmann::json p = condch.getPayloadChannel(channelKey);
    std::cout << "Payload for channel 1 is " << p.dump() << std::endl;
    condch.addIov(1000);
    // Get channel map string
    auto m_chmap = condch.getChannelMap();
    std::string chmap;
    nlohmann::json channelArray = nlohmann::json::array();
    for (const auto &[channelId, channelName] : m_chmap) {
      nlohmann::json channelObj;
      channelObj[channelId] = channelName;
      channelArray.push_back(channelObj);
    }
    chmap = channelArray.dump();

    std::cout << "Channel map is " << chmap << std::endl;
  } catch (CrestBaseException &ex) {
    cout << ex.what() << endl;
    cout << "Error occurred while processing CrestContainer" << endl;
    return -1;
  }
}
