#include <ersap/engine.hpp>
#include <ersap/stdlib/json_utils.hpp>
#include <ersap/engine_data_type.hpp>

#include <e2sar.hpp>

#include <iostream>

namespace ersap {
    namespace e2 {

        // class SampaEngine;

        class ReassemblerService : public ersap::Engine
        {
        public:
            ReassemblerService() = default;

            ReassemblerService(const ReassemblerService&) = delete;
            ReassemblerService& operator=(const ReassemblerService&) = delete;

            ReassemblerService(ReassemblerService&&) = default;
            ReassemblerService& operator=(ReassemblerService&&) = default;

            ~ReassemblerService() override = default;

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
            e2sar::Reassembler::ReassemblerFlags rflags;
            std::unique_ptr<e2sar::Reassembler> reas;
        };

    } // end namespace jana
} // end namespace ersap
