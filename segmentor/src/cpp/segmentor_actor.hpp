#include <ersap/engine.hpp>
#include <ersap/stdlib/json_utils.hpp>
#include <ersap/engine_data_type.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <e2sar.hpp>

#include <iostream>

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

            ~SegmentorService();

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
            void addSender(boost::property_tree::ptree);
            std::unique_ptr<e2sar::Segmenter> seg;
        };

    } // end namespace jana
} // end namespace ersap
