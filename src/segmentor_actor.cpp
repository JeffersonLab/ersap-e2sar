
#include <ersap/engine.hpp>
#include <ersap/stdlib/json_utils.hpp>

#include <e2sar.hpp>

#include <iostream>

extern "C"
namespace ersap {
    namespace e2sar {

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

namespace ersap {
    namespace e2sar {
        ersap::EngineData SegmentorService::configure(ersap::EngineData& input)
        {
            // Ersap provides a simple JSON parser to read configuration data
            // and configure the service.
            auto config = ersap::stdlib::parse_json(input);


            // TODO: Extract jana_config_file_name from ersap config
            auto jana_config_file_name = "config.e2sar";
            return {};
        }


        ersap::EngineData SegmentorService::execute(ersap::EngineData& input) {
            auto output = ersap::EngineData{};
            return output;
        }


        ersap::EngineData SegmentorService::execute_group(const std::vector<ersap::EngineData>& inputs)
        {
            auto output = ersap::EngineData{};
            return output;
        }

        std::vector<ersap::EngineDataType> SegmentorService::input_data_types() const
        {
            // TODO: Need to understand
            // return { ersap::type::JSON, ersap::type::BYTES };
            return { ersap::type::JSON, ersap::type::BYTES };
        }


        std::vector<ersap::EngineDataType> SegmentorService::output_data_types() const
        {
            // TODO: Need to understand
            // return { ersap::type::JSON, ersap::type::BYTES };
            return { ersap::type::JSON, ersap::type::BYTES };
        }


        std::set<std::string> SegmentorService::states() const
        {
            return std::set<std::string>{};
        }


        std::string SegmentorService::name() const
        {
            return "Segmentor Service";
        }


        std::string SegmentorService::author() const
        {
            return "Srinvias Sivakumar";
        }


        std::string SegmentorService::description() const
        {
            return "E2SAR example";
        }


        std::string SegmentorService::version() const
        {
            return "0.1";
        }
    }
}
extern "C"
std::unique_ptr<ersap::Engine> create_engine()
{
    return std::make_unique<ersap::e2sar::SegmentorService>();
}