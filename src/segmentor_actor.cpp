
#include <ersap/engine.hpp>
#include <ersap/stdlib/json_utils.hpp>
#include <ersap/engine_data_type.hpp>

#include <e2sar.hpp>

#include <iostream>

extern "C"
namespace ersap {
    namespace e2 {

        // class SampaEngine;

        class SegmentorService : public ersap::Engine
        {
        public:
            SegmentorService() = default;

            SegmentorService(const SegmentorService&) = delete;
            SegmentorService& operator=(const SegmentorService&) = delete;

            SegmentorService(SegmentorService&&) = default;
            SegmentorService& operator=(SegmentorService&&) = default;

            ~SegmentorService() override = default;

        public:
            ersap::EngineData configure(ersap::EngineData&) override;

            ersap::EngineData execute(ersap::EngineData&) override;

            ersap::EngineData execute_group(const std::vector<ersap::EngineData>&) override;

        public:
            std::vector<ersap::EngineDataType> input_data_types() const override;

            std::vector<ersap::EngineDataType> output_data_types() const override;

            std::set<std::string> states() const override;

        public:
            std::string name() const override;

            std::string author() const override;

            std::string description() const override;

            std::string version() const override;

        private:
            // TODO: Use smart pointers here
            // TODO: Deal with multiple threads calling configure() and clobbering JApplication
            // std::shared_ptr<SampaEngine> engine_{};
        };

    } // end namespace jana
} // end namespace ersap

ersap::EngineData ersap::e2::SegmentorService::configure(ersap::EngineData& input)
{
    // Ersap provides a simple JSON parser to read configuration data
    // and configure the service.
    auto config = ersap::stdlib::parse_json(input);
    std::cout << "Does this work?" << std::endl;
    std::cout << config.dump() << std::endl;

    

    // TODO: Extract jana_config_file_name from ersap config
    auto jana_config_file_name = "config.e2sar";
    return {};
}


ersap::EngineData ersap::e2::SegmentorService::execute(ersap::EngineData& input) {
    std::cout << "I should be processing input here" << std::endl;
    auto output = ersap::EngineData{};
    // std::cout << input.mime_type() << std::endl;
    if (input.mime_type() != ersap::type::BYTES) {
        output.set_status(ersap::EngineStatus::ERROR);
        output.set_description("Wrong input type");
        return output;
    }
    else{
        std::cout << input.mime_type() << std::endl;
    }
    std::cout << input.data().type().name() << std::endl;
    auto& event_input = ersap::data_cast<std::vector<std::uint8_t>>(input);

    std::string segUriString{"ejfat://useless@192.168.100.1:9876/lb/1?sync=192.168.254.1:12345&data=10.250.100.123"};
    e2sar::EjfatURI uri(segUriString);

    u_int16_t dataId = 0x0505;
    u_int32_t eventSrcId = 0x11223344;
    e2sar::Segmenter::SegmenterFlags sflags;
    sflags.syncPeriodMs = 1000; // in ms
    sflags.syncPeriods = 5; // number of sync periods to use for sync

    std::cout << "Running data test for 10 seconds against sync " << 
        uri.get_syncAddr().value().first << ":" << 
        uri.get_syncAddr().value().second << " and data " <<
        uri.get_dataAddrv4().value().first << ":" <<
        uri.get_dataAddrv4().value().second << 
        std::endl;

    // create a segmenter and start the threads
    e2sar::Segmenter seg(uri, dataId, eventSrcId, sflags);

    auto res = seg.openAndStart();

    if (res.has_error())
        std::cout << "Error encountered opening sockets and starting threads: " << res.error().message() << std::endl;
    output.set_data(ersap::type::BYTES, input.data());
    std::string eventString(event_input.begin(), event_input.end());
    std::cout << "The event data is string '" << eventString << "' of length " << eventString.length() << std::endl;
    //
    // send one event message per 2 seconds that fits into a single frame using event queue
    //
    auto sendStats = seg.getSendStats();
    if (sendStats.get<1>() != 0) 
    {
        std::cout << "Error encountered after opening send socket: " << strerror(sendStats.get<2>()) << std::endl;
    }
    for(auto i=0; i<5;i++) {
        auto sendres = seg.addToSendQueue(event_input.data(), event_input.size());
        sendStats = seg.getSendStats();
        if (sendStats.get<1>() != 0) 
        {
            std::cout << "Error encountered sending event frames: " << strerror(sendStats.get<2>()) << std::endl;
        }               
        // sleep for a second
        boost::this_thread::sleep_for(boost::chrono::seconds(2));
    }

    // check the sync stats
    auto syncStats = seg.getSyncStats();
    sendStats = seg.getSendStats();

    if (syncStats.get<1>() != 0) 
    {
        std::cout << "Error encountered sending sync frames: " << strerror(syncStats.get<2>()) << std::endl;
    }
    // send 10 sync messages and no errors
    std::cout << "Sent " << syncStats.get<0>() << " sync frames" << std::endl;

    // check the send stats
    std::cout << "Sent " << sendStats.get<0>() << " data frames" << std::endl;

    output.set_data(ersap::type::BYTES, input.data());
    return output;
}


ersap::EngineData ersap::e2::SegmentorService::execute_group(const std::vector<ersap::EngineData>& inputs)
{

    auto output = ersap::EngineData{};
    output.set_data(ersap::type::BYTES, NULL);
    return output;
}

std::vector<ersap::EngineDataType> ersap::e2::SegmentorService::input_data_types() const
{
    // TODO: Need to understand
    // return { ersap::type::JSON, ersap::type::BYTES };
    return { ersap::type::JSON, ersap::type::BYTES };
}


std::vector<ersap::EngineDataType> ersap::e2::SegmentorService::output_data_types() const
{
    // TODO: Need to understand
    // return { ersap::type::JSON, ersap::type::BYTES };
    return { ersap::type::JSON, ersap::type::BYTES };
}


std::set<std::string> ersap::e2::SegmentorService::states() const
{
    return std::set<std::string>{};
}


std::string ersap::e2::SegmentorService::name() const
{
    return "Segmentor Service";
}


std::string ersap::e2::SegmentorService::author() const
{
    return "Srinvias Sivakumar";
}


std::string ersap::e2::SegmentorService::description() const
{
    return "E2SAR example";
}


std::string ersap::e2::SegmentorService::version() const
{
    return "0.1";
}

extern "C"
std::unique_ptr<ersap::Engine> create_engine()
{
    return std::make_unique<ersap::e2::SegmentorService>();
}