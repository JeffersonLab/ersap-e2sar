#include "segmentor_actor.hpp"
#include <fstream>


namespace ersap {
    namespace e2 {
        ersap::EngineData SegmentorService::configure(ersap::EngineData& input)
        {
            // Ersap provides a simple JSON parser to read configuration data
            // and configure the service.
            auto config = ersap::stdlib::parse_json(input);
            std::string iniFile;
            u_int16_t dataId = 5555;
            u_int32_t eventSrcId = 7777;
            try{
                iniFile = ersap::stdlib::get_string(config, "INI_FILE");
            }
            catch (ersap::stdlib::JsonError){
                std::cout << "Could not parse config file:" << config.dump() << std::endl;
                exit(-1);
            }
            auto res = e2sar::Segmenter::SegmenterFlags::getFromINI(iniFile);
            e2sar::Segmenter::SegmenterFlags sflags = res.value();

            boost::property_tree::ptree param_tree;
            boost::property_tree::ini_parser::read_ini(iniFile, param_tree);
            

            if(sflags.useCP){
                addSender(param_tree);
            }

            std::string ejfatURI = param_tree.get<std::string>("lb-config.ejfatUri", "ejfaturi");
            e2sar::EjfatURI uri(ejfatURI, e2sar::EjfatURI::TokenType::instance);

            std::cout << "EJFAT_URI = " << ejfatURI << std::endl;

            seg = std::make_unique<e2sar::Segmenter>(uri, dataId, eventSrcId, sflags);

            auto res2 = seg->openAndStart();

            if (res2.has_error())
                std::cout << "Error encountered opening sockets and starting threads: " << res.error().message() << std::endl;

            return {};
        }

        void SegmentorService::addSender(boost::property_tree::ptree param_tree){
            std::string ejfatURI = param_tree.get<std::string>("lb-config.ejfatUri", "ejfaturi");
            e2sar::EjfatURI uri(ejfatURI, e2sar::EjfatURI::TokenType::instance);
            std::string ip = param_tree.get<std::string>("lb-config.senderIP", "127.0.0.1");
            bool validate = param_tree.get<bool>("lb-config.validate", true);
            bool preferHostAddress = param_tree.get<bool>("lb-config.preferHostAddress", false);

            std::cout << "EJFAT_URI = " << ejfatURI << std::endl;
            std::cout << "ipAddress = " << ip << std::endl;
            std::cout << "validate = " << validate << std::endl;
            std::cout << "preferHostAddress = " << preferHostAddress << std::endl;

            e2sar::LBManager lbman(uri, validate, preferHostAddress);
            std::cout << "Adding senders to LB: ";
            std::vector<std::string> senders;
            senders.push_back(ip);  
            for (auto s: senders)
                std::cout << ip << " ";
            std::cout << std::endl;
            auto addres = lbman.addSenders(senders);
            if (addres.has_error()) 
            {
                std::cerr << "Unable to add a sender due to error " << addres.error().message() 
                    << ", exiting" << std::endl;
                exit(-1);
            }
        }

        void freeBuffer(boost::any a) {
            auto p = boost::any_cast<uint8_t*>(a);
            delete[] p;
        }

        ersap::EngineData SegmentorService::execute(ersap::EngineData& input) {
            auto output = ersap::EngineData{};
            
            if (input.mime_type() != ersap::type::BYTES) {
                output.set_status(ersap::EngineStatus::ERROR);
                output.set_description("Wrong input type");
                return output;
            }

            auto& event_input = ersap::data_cast<std::vector<u_int8_t>>(input);
            uint8_t* byte_arr = new uint8_t[event_input.size()];
            std::copy(event_input.begin(),event_input.end(),byte_arr);
            std::string eventString(event_input.begin(), event_input.end());
            auto sendres = seg->addToSendQueue(byte_arr, event_input.size(),0LL,0,0,&freeBuffer,byte_arr);//0 is default
            output.set_data(ersap::type::BYTES, input.data());

            return output;
        }


        ersap::EngineData SegmentorService::execute_group(const std::vector<ersap::EngineData>& inputs)
        {   
            std::vector<std::vector<uint8_t>> byteOutput;
            auto output = ersap::EngineData{};
            for(auto input : inputs){
                auto res = execute(input);
                byteOutput.push_back(ersap::data_cast<std::vector<std::uint8_t>>(res));
            }
            output.set_data(ersap::type::BYTES, byteOutput.data());
            return output;
        }

        std::vector<ersap::EngineDataType> SegmentorService::input_data_types() const
        {
            return { ersap::type::JSON, ersap::type::BYTES };
        }


        std::vector<ersap::EngineDataType> SegmentorService::output_data_types() const
        {
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

        SegmentorService::~SegmentorService(){
            // check the sync stats
            auto syncStats = seg->getSyncStats();
            auto sendStats = seg->getSendStats();

            std::cout << "Sent " << syncStats.get<0>() << " sync frames" << std::endl;
            std::cout << "Sent " << sendStats.get<0>() << " data frames" << std::endl;
        }
    }
}


extern "C"
std::unique_ptr<ersap::Engine> create_engine()
{
    return std::make_unique<ersap::e2::SegmentorService>();
}