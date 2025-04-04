```c++
typedef struct _OTAInfo
{
  AppCtx *appCtx; 
  // override_config.primary_gie_config.enable
  // pipeline.common_elements.primary_gie_bin.primary_gie
  // override_config.primary_gie_config.model_engine_file_path
  // g_object_set (G_OBJECT (primary_gie), "model-engine-file", model_engine_file_path, NULL);


  // from thread
  // ota_inotify_fd =  inotify_init ()
  // ota_watch_desc =  inotify_add_watch (ota_appCtx->ota_inotify_fd, ota_dir, IN_ALL_EVENTS);
  // ota_appCtx->pipeline.common_elements.primary_gie_bin.primary_gie

  // g_signal_connect (G_OBJECT (primary_gie), "model-updated", G_CALLBACK (infer_model_updated_cb), NULL);
  // infer_model_updated_cb


  // https://man7.org/linux/man-pages/man7/inotify.7.html
  // struct inotify_event *event = (struct inotify_event *) &buffer[i];
  // if IN_MOVED_TO +  strstr ("..data", event->name)
  // ota_appCtx->override_config
  // if IN_CLOSE_WRITE

  // parse_config_file (&ota_appCtx->override_config, ota_ds_config_file)
  // parse_config_file_yaml (&ota_appCtx->override_config, ota_ds_config_file)
  // apply_ota (ota_appCtx);
  gchar *override_cfg_file;
} OTAInfo;

/**
 * @brief  Performs model update OTA operation
 *         Sets "model-engine-file" configuration parameter
 *         on infer plugin to initiate model switch OTA process
 * @param  ota_appCtx [IN] App context pointer
 */
void apply_ota (AppCtx * ota_appCtx) {
      GstElement *primary_gie = NULL;

  if (ota_appCtx->override_config.primary_gie_config.enable) {
    primary_gie =
        ota_appCtx->pipeline.common_elements.primary_gie_bin.primary_gie;
    gchar *model_engine_file_path =
        ota_appCtx->override_config.primary_gie_config.model_engine_file_path;

    gettimeofday (&ota_request_time, NULL);
    if (model_engine_file_path) {
      g_print ("\nNew Model Update Request %s ----> %s\n",
          GST_ELEMENT_NAME (primary_gie), model_engine_file_path);
      g_object_set (G_OBJECT (primary_gie), "model-engine-file",
          model_engine_file_path, NULL);
    } else {
      g_print
          ("\nInvalid New Model Update Request received. Property model-engine-path is not set\n");
    }
  }
}

/**
 * @brief  Thread which handles the model-update OTA functionlity
 *         1) Adds watch on the changes made in the provided ota-override-file,
 *            if changes are detected, validate the model-update change request,
 *            intiate model-update OTA process
 *         2) Frame drops / frames without inference should NOT be detected in
 *            this on-the-fly model update process
 *         3) In case of model update OTA fails, error message will be printed
 *            on the console and pipeline continues to run with older
 *            model configuration
 * @param  gpointer [IN] Pointer to OTAInfo structure
 * @param  gpointer [OUT] Returns NULL in case of thread exits
 */
/**
 * Independent thread to perform model-update OTA process based on the inotify events
 * It handles currently two scenarios
 * 1) Local Model Update Request (e.g. Standalone Appliation)
 *    In this case, notifier handler watches for the ota_override_file changes
 * 2) Cloud Model Update Request (e.g. EGX with Kubernetes)
 *    In this case, notifier handler watches for the ota_override_file changes along with
 *    ..data directory which gets mounted by EGX deployment in Kubernetes environment.
 */
gpointer
ota_handler_thread (gpointer data)
{

  int length, i = 0;
  char buffer[INOTIFY_EVENT_BUF_LEN];
  OTAInfo *ota = (OTAInfo *) data;
  gchar *ota_ds_config_file = ota->override_cfg_file;
  AppCtx *ota_appCtx = ota->appCtx;
  struct stat file_stat = { 0 };
  GstElement *primary_gie = NULL;
  gboolean connect_pgie_signal = FALSE;

  ota_appCtx->ota_inotify_fd = inotify_init ();

  if (ota_appCtx->ota_inotify_fd < 0) {
    perror ("inotify_init");
    return NULL;
  }

  char *real_path_ds_config_file = realpath (ota_ds_config_file, NULL);
  g_print ("REAL PATH = %s\n", real_path_ds_config_file);

  gchar *ota_dir = g_path_get_dirname (real_path_ds_config_file);
  ota_appCtx->ota_watch_desc =
      inotify_add_watch (ota_appCtx->ota_inotify_fd, ota_dir, IN_ALL_EVENTS);

  int ret = lstat (ota_ds_config_file, &file_stat);
  ret = ret;

  if (S_ISLNK (file_stat.st_mode)) {
    printf (" Override File Provided is Soft Link\n");
    gchar *parent_ota_dir = g_strdup_printf ("%s/..", ota_dir);
    ota_appCtx->ota_watch_desc =
        inotify_add_watch (ota_appCtx->ota_inotify_fd, parent_ota_dir,
        IN_ALL_EVENTS);
  }

  while (1) {
    i = 0;
    length = read (ota_appCtx->ota_inotify_fd, buffer, INOTIFY_EVENT_BUF_LEN);

    if (length < 0) {
      perror ("read");
    }

    if (quit == TRUE)
      goto done;

    while (i < length) {
      struct inotify_event *event = (struct inotify_event *) &buffer[i];

      // Enable below function to print the inotify events, used for debugging purpose
      if (0) {
        display_inotify_event (event);
      }

      if (connect_pgie_signal == FALSE) {
        primary_gie =
            ota_appCtx->pipeline.common_elements.primary_gie_bin.primary_gie;
        if (primary_gie) {
          g_signal_connect (G_OBJECT (primary_gie), "model-updated",
              G_CALLBACK (infer_model_updated_cb), NULL);
          connect_pgie_signal = TRUE;
        } else {
          printf
              ("Gstreamer pipeline element nvinfer is yet to be created or invalid\n");
          continue;
        }
      }

      if (event->len) {
        if (event->mask & IN_MOVED_TO) {
          if (strstr ("..data", event->name)) {
            memset (&ota_appCtx->override_config, 0,
                sizeof (ota_appCtx->override_config));
            if (!IS_YAML(ota_ds_config_file)) {
              if (!parse_config_file (&ota_appCtx->override_config,
                      ota_ds_config_file)) {
                NVGSTDS_ERR_MSG_V ("Failed to parse config file '%s'",
                    ota_ds_config_file);
                g_print
                    ("Error: ota_handler_thread: Failed to parse config file '%s'",
                    ota_ds_config_file);
              } else {
                apply_ota (ota_appCtx);
              }
            } else if (IS_YAML(ota_ds_config_file)) {
                if (!parse_config_file_yaml (&ota_appCtx->override_config,
                      ota_ds_config_file)) {
                NVGSTDS_ERR_MSG_V ("Failed to parse config file '%s'",
                    ota_ds_config_file);
                g_print
                    ("Error: ota_handler_thread: Failed to parse config file '%s'",
                    ota_ds_config_file);
              } else {
                apply_ota (ota_appCtx);
              }
            }
          }
        }
        if (event->mask & IN_CLOSE_WRITE) {
          if (!(event->mask & IN_ISDIR)) {
            if (strstr (ota_ds_config_file, event->name)) {
              g_print ("File %s modified.\n", event->name);

              memset (&ota_appCtx->override_config, 0,
                  sizeof (ota_appCtx->override_config));
              if (!IS_YAML(ota_ds_config_file)) {
                if (!parse_config_file (&ota_appCtx->override_config,
                        ota_ds_config_file)) {
                  NVGSTDS_ERR_MSG_V ("Failed to parse config file '%s'",
                      ota_ds_config_file);
                  g_print
                      ("Error: ota_handler_thread: Failed to parse config file '%s'",
                      ota_ds_config_file);
                } else {
                  apply_ota (ota_appCtx);
                }
              } else if (IS_YAML(ota_ds_config_file)) {
                  if (!parse_config_file_yaml (&ota_appCtx->override_config,
                        ota_ds_config_file)) {
                  NVGSTDS_ERR_MSG_V ("Failed to parse config file '%s'",
                      ota_ds_config_file);
                  g_print
                      ("Error: ota_handler_thread: Failed to parse config file '%s'",
                      ota_ds_config_file);
                } else {
                  apply_ota (ota_appCtx);
                }
              }
            }
          }
        }
      }
      i += INOTIFY_EVENT_SIZE + event->len;
    }
  }
done:
  inotify_rm_watch (ota_appCtx->ota_inotify_fd, ota_appCtx->ota_watch_desc);
  close (ota_appCtx->ota_inotify_fd);

  free (real_path_ds_config_file);
  g_free (ota_dir);

  g_free (ota);
  return NULL;
}


/**
 * Callback function to notify the status of the model update
 */
static void
infer_model_updated_cb (GstElement * gie, gint err, const gchar * config_file)
{
  double otaTime = 0;
  gettimeofday (&ota_completion_time, NULL);

  otaTime = (ota_completion_time.tv_sec - ota_request_time.tv_sec) * 1000.0;
  otaTime += (ota_completion_time.tv_usec - ota_request_time.tv_usec) / 1000.0;

  const char *err_str = (err == 0 ? "ok" : "failed");
  g_print
      ("\nModel Update Status: Updated model : %s, OTATime = %f ms, result: %s \n\n",
      config_file, otaTime, err_str);
}
```


gstnvinfer.cpp only has 2 modifiable at runtime parameters that are defined by GST_PARAM_MUTABLE_PLAYING

"config-file-path"
"model-engine-file"

/* Function called when a property of the element is set. Standard boilerplate.
 */
static void
gst_nvinfer_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)

    impl->triggerNewModel (cfg_path, MODEL_LOAD_FROM_CONFIG);
    impl->triggerNewModel (engine_path, MODEL_LOAD_FROM_ENGINE);







# Service Maker implementation

```c++
      std::string model_engine_file;
      std::string pgie = "pgie";
      pipeline[pgie].getProperty("model-engine-file", model_engine_file); 

      // create model update watcher
      otf_triggers.push_back(std::make_unique<NvDsModelEngineWatchOTFTrigger>(&pipeline[pgie], model_engine_file));
      NvDsModelEngineWatchOTFTrigger& otf_trigger = *otf_triggers.back().get();


       pipeline.start([&otf_trigger](Pipeline &p, const Pipeline::Message &msg) {
        const Pipeline::StateTransitionMessage *state_change_msg = dynamic_cast<const Pipeline::StateTransitionMessage*>(&msg);
        // handle state change message
        if (state_change_msg) {
          static bool otf_trigger_started = false;
          if (!otf_trigger_started) {
            otf_trigger.start();
            otf_trigger_started = true;
          }
        }


// model_engine_watch_otf_trigger.hpp
      class NvDsModelEngineWatchOTFTrigger {
    public:
    NvDsModelEngineWatchOTFTrigger (Element *infer, const std::string watch_file)
    :infer_(infer), watch_file_path_(watch_file) {}

    ~NvDsModelEngineWatchOTFTrigger () {
        stop();
    }
    bool start();
    bool stop();
    void file_watch_thread_func();
    bool stop_watch = false;

    int ota_inotify_fd_;
    std::thread file_watch_thread_;
    Element *infer_;
    std::string watch_file_path_;
};

```