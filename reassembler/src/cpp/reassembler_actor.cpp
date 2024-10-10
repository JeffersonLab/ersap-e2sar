#include "reassembler_actor.hpp"

extern "C"
namespace ersap {
    namespace e2 {
        ersap::EngineData ReassemblerService::configure(ersap::EngineData& input)
        {
            // Ersap provides a simple JSON parser to read configuration data
            // and configure the service.
            auto config = ersap::stdlib::parse_json(input);

            std::string iniFile;

            try{
                iniFile = ersap::stdlib::get_string(config, "INI_FILE");
            }
            catch (ersap::stdlib::JsonError){
                std::cout << "Could not parse config file:" << config.dump() << std::endl;
                exit(-1);
            }

            auto res = e2sar::Reassembler::ReassemblerFlags::getFromINI(iniFile);
            e2sar::Reassembler::ReassemblerFlags rflags = res.value();
            
            boost::property_tree::ptree param_tree;
            boost::property_tree::ini_parser::read_ini(iniFile, param_tree);

            std::string ejfatURI = param_tree.get<std::string>("lb-config.ejfatUri", "");
            std::string ipAddress = param_tree.get<std::string>("lb-config.ip", "127.0.0.1");
            u_int16_t listen_port = param_tree.get<u_int16_t>("lb-config.port", 10000);
            initTimeout = param_tree.get<u_int16_t>("lb-config.initTimoeut", 20000);
            timeout = param_tree.get<u_int16_t>("lb-config.timeout", 1000);
            std::cout << "EJFAT_URI = " << ejfatURI << std::endl;
            std::cout << "ipAddress = " << ipAddress << std::endl;
            std::cout << "listen_port = " << listen_port << std::endl;

            e2sar::EjfatURI reasUri(ejfatURI, e2sar::EjfatURI::TokenType::instance);


            

            if(rflags.useCP){
                try{
                    boost::asio::ip::address recv_ip = boost::asio::ip::make_address(ipAddress);
                    reas = std::make_unique<e2sar::Reassembler>(reasUri, recv_ip, listen_port, 1, rflags);
                    registerWorker(param_tree);
                }
                catch(e2sar::E2SARException e){
                    std::string errorMsg = e;
                    std::cout << errorMsg << std::endl;
                    exit(-1);
                }
            }
            std::cout << "After registering" << std::endl;
            auto res2 = reas->openAndStart();
            if (res2.has_error()){
                std::cout << "Error encountered opening sockets and starting reassembler threads: " << res2.error().message() << std::endl;
                exit(-1);
            }
            return {};
        }

        void ReassemblerService::registerWorker(boost::property_tree::ptree param_tree){
            auto hostname_res = e2sar::NetUtil::getHostName();
            if (hostname_res.has_error()) 
            {   
                std::cout << "Could not resolve hostName" << hostname_res.error().code() << hostname_res.error().message() << std::endl;
                exit(-1);
            }
            std::cout << "Hostname = " << hostname_res.value() << std::endl;
            auto regres = reas->registerWorker(hostname_res.value());
            if (regres.has_error())
            {
                std:: cout << "Unable to register worker node due to " << regres.error().message() << std::endl;
                exit(-1);
            }
            if (regres.value() == 1)
                std::cout << "Registered the worker" << std::endl;

            std::cout << "This reassembler has " << reas->get_numRecvThreads() << " receive threads and is listening on ports " << 
                reas->get_recvPorts().first << ":" << reas->get_recvPorts().second << " using portRange " << reas->get_portRange() << 
                std::endl;
        }

        ersap::EngineData ReassemblerService::execute(ersap::EngineData& input) {
            auto output = ersap::EngineData{};
            
            if (input.mime_type() != ersap::type::SINT32) {
                output.set_status(ersap::EngineStatus::ERROR);
                output.set_description("Wrong input type");
                return output;
            }
            auto& event_type = ersap::data_cast<std::int32_t>(input);
            std::vector<uint8_t> output_events;


            u_int8_t *eventBuf{nullptr};
            size_t eventLen;
            e2sar::EventNum_t eventNum;
            u_int16_t recDataId;

            uint16_t e2sarTimeout = timeout;
            if(!event_type)
                e2sarTimeout = initTimeout;

            
            auto recvres = reas->recvEvent(&eventBuf, &eventLen, &eventNum, &recDataId, timeout);
            if (recvres.has_error())
                std::cout << "Error encountered receiving event frames " << std::endl;
            if (recvres.value() == -1)
                std::cout << "No message received, continuing" << std::endl;
            else{
                std::cout << "Received message: " << reinterpret_cast<char*>(eventBuf) << " of length " << eventLen << " with event number " << eventNum << " and data id " << recDataId << std::endl;
                output_events = std::vector<uint8_t>(eventBuf,eventBuf+eventLen);
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