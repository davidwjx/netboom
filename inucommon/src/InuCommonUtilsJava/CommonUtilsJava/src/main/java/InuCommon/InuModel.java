package InuCommon;

/**
 * Java wrapper to Sensor model
 * 
 * Role: Each Sensor model has pre-defined parameters that are loaded from internal config file
 * 
 * Responsibilities: 
 * 1. Knows all parameters that are associated with Sensor model
 * 2. Knows how to load these parameters from internal config file
 * 
 * @author Olga
 * @since 2.09.0004
 */


public class InuModel {

	public InuModel()
	{
		defaultDepthResolution  = ESensorResolution.eUnknownResolution;
		defaultWebCamResolution  = ESensorResolution.eUnknownResolution;
//		defaultVideoFormat = ETiffFormat.eEmpty;
//		defaultWebcamFormat = ETiffFormat.eEmpty;
		modelName = new String();
//		imgStreamChannels = new String();		
	}
	
	public native boolean init(String iSensorName);

    /**
     * Current object unique name (identifier)   
     */
	private String modelName;
    public String getModelName() {return modelName;}
    
    /**
     * Model description
     */
    private String description;
    public String getDescription() {return description;}

    /**
     * Model description
     */
//    private String imgStreamChannels;
//    public String getImgStreamChannels() {return imgStreamChannels;}

    /**
     *  Max Exposure Time value that is allowed to set to this model 
     */
//    private long maxETvalue;
//    public long getMaxETvalue() {return maxETvalue;}
    
    /** 
     * Exposure Time Register address
     */
    private long ETRegAddress;
    public long getETRegAddress() {return ETRegAddress;}
    
    /** 
     * Exposure Time Register size 
     */
    private long ETRegSize;  
    public long getETRegSize() {return ETRegSize;}
   
    /** 
     * FW unique ID for that model (real value is INU_DEFSG_sensorModelE)
     */
    private long FWEnumerator;
    public long getFWEnumerator() {return FWEnumerator;}
   
    /** 
     * Default Depth / Video FPS that should be used for this model
     */
    private long defaultVideoFPS;
    public long getDefaultDepthFPS() {return defaultVideoFPS;}

    /** 
     * Default WebCam FPS that should be used for this model
     */
    private long defaultWebCamFPS;
    public long getDefaultWebCamFPS() {return defaultWebCamFPS;}

    /**
     * Default Interleaved mode that should be used for this model
     */
    private boolean defaultInterleavedMode;
    public boolean getDefaultInterleavedMode() {return defaultInterleavedMode;}
   
    /**
     * Default Mirror mode for display
     */
//    private boolean defaultMirrorMode;
//    public boolean getDefaultMirrorMode() {return defaultMirrorMode;}

    /**
     * True if automatic Sensor Control should be used.
     */
    private boolean defaultAutoControl;
    public boolean getDefaultAutoControl() {return defaultAutoControl;}

    /** 
     * Default Video format (Y Only or YUV packed)
     */
//    private ETiffFormat defaultVideoFormat;
//    public ETiffFormat getDefaultVideoFormat() {return defaultVideoFormat;}

    /** 
     * Default Video format (RGB565, Y or None when Webcam isn't available)
     */
//    private ETiffFormat defaultWebcamFormat;
//    public ETiffFormat getDefaultWebcamFormat() {return defaultWebcamFormat;}

    /** 
     * Default resolution that should be used for this model's depth/video
     */
    private ESensorResolution defaultDepthResolution;
    public ESensorResolution getDefaultDepthResolution() {return defaultDepthResolution;}

    /** 
     * Default resolution that should be used for this model's webcam
     */
    private ESensorResolution defaultWebCamResolution;
    public ESensorResolution getDefaultWebCamResolution() {return defaultWebCamResolution;}

    /** 
     * True if multi channels is supported by current model.
     */
//    private boolean multiChannelsIsSupported;
//    public boolean getMultiChannelsIsSupported() {return multiChannelsIsSupported;}

    /**
     * True if this model allows feature tracking
     */
//    private boolean featuresTrackingSupport;
//    public boolean getFeaturesTrackingSupport() {return featuresTrackingSupport;}
    
	private long nativeInstance;
	private boolean isDisposed;
    
}
