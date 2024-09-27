#include "reassembler_actor.hpp"

extern "C"
namespace ersap {
    namespace e2 {
        ersap::EngineData ReassemblerService::configure(ersap::EngineData& input)
        {
            // Ersap provides a simple JSON parser to read configuration data
            // and configure the service.
            auto config = ersap::stdlib::parse_json(input);

            std::string ejfatURI;
            bool useCP; 
            bool withLBHeader; 

            try{
                ejfatURI = ersap::stdlib::get_string(config, "EJFAT_URI");
                useCP = ersap::stdlib::get_bool(config, "USE_CP");
                withLBHeader = ersap::stdlib::get_bool(config, "LB_HEADER");
                std::cout << "Parsed ejfatURI from config: " << ejfatURI << std::endl;
            }
            catch (ersap::stdlib::JsonError){
                std::cout << "Could not parse config file:" << config.dump() << std::endl;
                exit(-1);
            }
            e2sar::EjfatURI reasUri(ejfatURI, e2sar::EjfatURI::TokenType::instance);
            e2sar::Reassembler::ReassemblerFlags rflags;
            rflags.useCP = useCP; 
            rflags.withLBHeader = true; 
            reas = std::make_unique<e2sar::Reassembler>(reasUri, 1, rflags);
            return {};
        }


        ersap::EngineData ReassemblerService::execute(ersap::EngineData& input) {
            auto output = ersap::EngineData{};
            
            if (input.mime_type() != ersap::type::SINT32) {
                output.set_status(ersap::EngineStatus::ERROR);
                output.set_description("Wrong input type");
                return output;
            }
            auto& buffer_len = ersap::data_cast<std::int32_t>(input);
            std::string reasUriString{"ejfat://useless@192.168.100.1:9876/lb/1?sync=192.168.0.1:12345&data=127.0.0.1"};
            std::vector<uint8_t> output_events;

            std::cout << "This reassembler has " << reas->get_numRecvThreads() << " receive threads and is listening on ports " << 
                reas->get_recvPorts().first << ":" << reas->get_recvPorts().second << " using portRange " << reas->get_portRange() << 
                std::endl;

            auto res2 = reas->openAndStart();
            if (res2.has_error())
                std::cout << "Error encountered opening sockets and starting reassembler threads: " << res2.error().message() << std::endl;

            u_int8_t *eventBuf{nullptr};
            size_t eventLen;
            e2sar::EventNum_t eventNum;
            u_int16_t recDataId;

            // receive data from the queue
            for(auto i = 0; i < buffer_len; i++)
            {
                auto recvres = reas->recvEvent(&eventBuf, &eventLen, &eventNum, &recDataId, 10000);
                if (recvres.has_error())
                    std::cout << "Error encountered receiving event frames " << std::endl;
                if (recvres.value() == -1)
                    std::cout << "No message received, continuing" << std::endl;
                else{
                    std::cout << "Received message: " << reinterpret_cast<char*>(eventBuf) << " of length " << eventLen << " with event number " << eventNum << " and data id " << recDataId << std::endl;
                    output_events.insert(output_events.end(),eventBuf,eventBuf+eventLen);
                }
                    
            }
            
            output.set_data(ersap::type::BYTES, output_events);
            return output;
        }


        ersap::EngineData ReassemblerService::execute_group(const std::vector<ersap::EngineData>& inputs)
        {

            auto output = ersap::EngineData{};
            output.set_data(ersap::type::BYTES, inputs.data());
            return output;
        }

        std::vector<ersap::EngineDataType> ReassemblerService::input_data_types() const
        {
            // TODO: Need to understand
            // return { ersap::type::JSON, ersap::type::BYTES };
            return { ersap::type::JSON, ersap::type::SINT32 };
        }


        std::vector<ersap::EngineDataType> ReassemblerService::output_data_types() const
        {
            // TODO: Need to understand
            // return { ersap::type::JSON, ersap::type::BYTES };
            return { ersap::type::JSON, ersap::type::BYTES };
        }


        std::set<std::string> ReassemblerService::states() const
        {
            return std::set<std::string>{};
        }


        std::string ReassemblerService::name() const
        {
            return "Reassemblee Service";
        }


        std::string ReassemblerService::author() const
        {
            return "Srinvias Sivakumar";
        }


        std::string ReassemblerService::description() const
        {
            return "E2SAR example";
        }


        std::string ReassemblerService::version() const
        {
            return "0.1";
        }

    }
}


extern "C"
std::unique_ptr<ersap::Engine> create_engine()
{
    return std::make_unique<ersap::e2::ReassemblerService>();
}