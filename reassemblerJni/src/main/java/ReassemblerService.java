import org.jlab.epsci.ersap.engine.EngineDataType;
import org.jlab.epsci.ersap.std.services.AbstractEventReaderService;
import org.jlab.epsci.ersap.std.services.EventReaderException;
import org.json.JSONObject;
import org.jlab.hpdf.EjfatURI;
import org.jlab.hpdf.Reassembler;
import org.jlab.hpdf.exceptions.E2sarNativeException;
import org.jlab.hpdf.messages.ReassembledEvent;

import java.io.FileNotFoundException;  // Import this class to handle errors
import java.io.BufferedReader;
import java.io.FileReader;

import java.util.Optional;

import java.io.IOException;
import java.net.InetAddress;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.file.Path;
public class ReassemblerService extends AbstractEventReaderService<BufferedReader> {


    private static boolean runinng;
    static{
        runinng = true;
        System.loadLibrary("jnie2sar");
    }

    private int count;
    private int timeout;
    private Reassembler reassembler;
    /**
     * Creates a new image reader service.
     */
    public ReassemblerService() {
        count = 1000;
        Thread shutdownHook = new Thread(() -> {
            System.out.println("Shutdown hook executed. Performing cleanup...");
            ReassemblerService.runinng = false;
        });

        // Register the shutdown hook
        Runtime.getRuntime().addShutdownHook(shutdownHook); 
    }

    @Override
    protected BufferedReader createReader(Path file, JSONObject opts) throws EventReaderException {
        try {
            BufferedReader FileReader =  new BufferedReader(new FileReader(file.toFile()));
            String iniFile = opts.optString("INI_FILE", "reassembler.ini");
            String stringUri = opts.getString("EJFAT_URI");
            String workerIp = opts.optString("WORKER_IP", "127.0.0.1");
            String hostname = opts.optString("HOSTNAME", "dummy");

            int listenPort = opts.optInt("WORKER_PORT", 10000);
            timeout = opts.optInt("TIMEOUT", 500);
            int threads = opts.optInt("REASSEMBLER_THREADS", 1);

            boolean useCp = opts.optBoolean("USE_CP", false);
            boolean preferV6 = opts.optBoolean("PREFER_V6", false);

            System.out.println("INI_FILE=" + iniFile);
            System.out.println("EJFAT_URI= " + stringUri);
            System.out.println("WORKER_IP= " + workerIp);
            System.out.println("WORKER_PORT= " + listenPort);
            System.out.println("PREFER_V6= " + preferV6);
            System.out.println("REASSEMBLER_THREADS= " + threads);

            EjfatURI dpUri = EjfatURI.createInstance(stringUri, EjfatURI.Token.INSTANCE, preferV6);
            reassembler = new Reassembler(dpUri, InetAddress.getByName(workerIp), listenPort, threads, iniFile);
            dpUri.free();
            if(useCp){
                registerWorker(hostname);
            }
            reassembler.openAndStart();
            return FileReader;
        }
        catch (FileNotFoundException e) {
            throw new EventReaderException("Could not create reader", e);
        }
        catch (IOException e){
            throw new EventReaderException("Unable to read file, e");
        }
        catch(E2sarNativeException e){
            throw new EventReaderException("Error occured in E2SAR-JAVA", e);
        }
    }

    void registerWorker(String hostName)throws EventReaderException{
        try {
            reassembler.registerWorker(hostName);
            System.out.println("Registered Worker");
        } catch (E2sarNativeException e) {
            throw new EventReaderException("Could not registerWorker", e);
        }

    }

    @Override
    protected void closeReader() {
        try {
            reassembler.free();
            reader.close();
        } catch (Exception e) {
            // ignore
        }
    }

    @Override
    protected int readEventCount() throws EventReaderException {
        return count;
    }

    @Override
    protected ByteOrder readByteOrder() throws EventReaderException {
        return ByteOrder.BIG_ENDIAN;
    }

    @Override
    protected Object readEvent(int eventNumber) throws EventReaderException {
        Optional<ReassembledEvent> reassembledEvent = Optional.empty();
        
        while(!reassembledEvent.isPresent() && runinng){
            reassembledEvent = reassembler.recvEvent(timeout);
        }
        if(reassembledEvent.isEmpty()){//If shutdown hoook is called then this will be empty.
            return ByteBuffer.allocateDirect(1);
        }
        ReassembledEvent event = reassembledEvent.get();
        ByteBuffer cloneBuffer = ByteBuffer.allocate(event.byteBuffer.capacity());
        cloneBuffer.put(event.byteBuffer);
        cloneBuffer.flip();
        reassembler.freeDirectBytebBuffer(event);
        return cloneBuffer;
    }

    @Override
    protected EngineDataType getDataType() {
        return EngineDataType.BYTES;
    }
}