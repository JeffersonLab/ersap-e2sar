import org.jlab.epsci.ersap.engine.EngineData;
import org.jlab.epsci.ersap.engine.EngineDataType;
import org.jlab.epsci.ersap.std.services.AbstractEventReaderService;
import org.jlab.epsci.ersap.std.services.EventReaderException;
import org.json.JSONObject;

import java.io.File;  // Import the File class
import java.io.FileNotFoundException;  // Import this class to handle errors
import java.util.Arrays;
import java.io.BufferedReader;
import java.io.FileReader;


import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.ByteOrder;
import java.nio.ByteBuffer;
import java.nio.file.Path;
import java.nio.file.Paths;
public class ByteBufferInputService extends AbstractEventReaderService<BufferedReader> {

    int count;
    int eventSize;
    byte[] startEvent;
    byte[] endEvent;
    /**
     * Creates a new image reader service.
     */
    public ByteBufferInputService() {
        count = 0;
        eventSize = 0;
        String startStr = "Start of event ..";
        String endStr = "End of Event.";
        startEvent = startStr.getBytes(Charset.forName("UTF-8"));
        endEvent = endStr.getBytes(Charset.forName("UTF-8"));
    }

    @Override
    protected BufferedReader createReader(Path file, JSONObject opts) throws EventReaderException {
        try {
            BufferedReader fileReader = new BufferedReader(new FileReader(file.toFile()));
            count = opts.optInt("events", 1000);
            eventSize = opts.optInt("eventSize", 1024);
            return fileReader;
        }
        catch (FileNotFoundException e) {
            throw new EventReaderException("Could not create reader", e);
        }
    }

    @Override
    protected void closeReader() {
        try {
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
        ByteBuffer buffer = ByteBuffer.allocate(eventSize);
        buffer.position(0);
        buffer.put(startEvent);
        buffer.position(buffer.capacity() - (endEvent.length + 1));
        buffer.put(endEvent);

        return buffer;
    }

    @Override
    protected EngineDataType getDataType() {
        return EngineDataType.BYTES;
    }
}