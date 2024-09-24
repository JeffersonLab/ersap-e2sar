
#include <ersap/engine.hpp>
#include <ersap/stdlib/json_utils.hpp>

#include <e2sar.hpp>

#include <iostream>

extern "C"
namespace ersap {
    namespace e2sar {

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
            ersap::EngineData configure(ersap::EngineData& input) {
                auto config = ersap::stdlib::parse_json(input);


                // TODO: Extract jana_config_file_name from ersap config
                auto jana_config_file_name = "config.ersap";

                //Need to configure segmentor here

            }

            ersap::EngineData execute(ersap::EngineData& input) {
                auto output = ersap::EngineData{};
                return output;
            }   

            ersap::EngineData execute_group(const std::vector<ersap::EngineData>& inputs) {
                auto output = ersap::EngineData{};
                return output;
            }

        public:
            std::vector<ersap::EngineDataType> input_data_types() {
                return { ersap::type::JSON, ersap::type::BYTES };
            }

            std::vector<ersap::EngineDataType> output_data_types(){
                return { ersap::type::JSON, ersap::type::BYTES }; 
            }

            std::set<std::string> states() const override;

        public:
            std::string name() {
                return "segmentor";
            };

            std::string author(){
                return "Srinivas Sivakumar";
            }

            std::string description(){
                return  "desc";
            }

            std::string version(){
                return "0.0.1";
            }

        private:
            // TODO: Use smart pointers here
            // TODO: Deal with multiple threads calling configure() and clobbering JApplication
            // std::shared_ptr<SampaEngine> engine_{};
        };


    }
}

std::unique_ptr<ersap::Engine> create_engine()
{
    return std::make_unique<ersap::e2sar::SegmentorService>();
}