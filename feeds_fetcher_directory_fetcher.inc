diff --git a/feeds_fetcher_directory_fetcher.inc b/feeds_fetcher_directory_fetcher.inc
index b48c878..f0acf31 100644
--- a/feeds_fetcher_directory_fetcher.inc
+++ b/feeds_fetcher_directory_fetcher.inc
@@ -79,7 +79,7 @@ class feeds_fetcher_directory_fetcher extends FeedsFileFetcher {
       '#type' => 'textfield',
       '#title' => t('Directory'),
       '#description' => t('Specify a URI to scan for feed data, e.g. private://files-to-scan.'),
-      '#default_value' => isset($source_config['source']) ? $source_config['source'] : '',
+      '#default_value' => (isset($source_config['source']) && !empty($source_config['source'])) ? $source_config['source'] : $this->config['directory'],
       '#required' => TRUE,
     );
     $form['reset'] = array(
@@ -116,8 +116,12 @@ class feeds_fetcher_directory_fetcher extends FeedsFileFetcher {
    * Override parent::configDefaults().
    */
   public function configDefaults() {
+    $schemes = $this->getSchemes();
+    $scheme = in_array('private', $schemes) ? 'private' : 'public';
+
     return array(
       'recursive' => TRUE,
+      'directory' => $scheme . '://feeds',
       'filemask' => '/\.txt$/',
       'updated_files' => FALSE,
     );
@@ -128,6 +132,16 @@ class feeds_fetcher_directory_fetcher extends FeedsFileFetcher {
    */
   public function configForm(&$form_state) {
     $form = array();
+    $form['directory'] = array(
+      '#type' => 'textfield',
+      '#title' => t('Upload directory'),
+      '#description' => t('Directory to search for files. Prefix the path with a scheme. Available schemes: @schemes.', array('@schemes' => implode(', ', $this->getSchemes()))),
+      '#default_value' => $this->config['directory'],
+      '#states' => array(
+        'visible' => array(':input[name="direct"]' => array('checked' => FALSE)),
+        'required' => array(':input[name="direct"]' => array('checked' => FALSE)),
+      ),
+    );
     $form['filemask'] = array(
       '#type' =>'textfield',
       '#title' => t('File mask'),
@@ -147,4 +161,30 @@ class feeds_fetcher_directory_fetcher extends FeedsFileFetcher {
     );
     return $form;
   }
+
+  /**
+   * Overrides parent::configFormValidate().
+   *
+   * Ensure that the chosen directory is accessible.
+   */
+  public function configFormValidate(&$values) {
+
+    $values['directory'] = trim($values['directory']);
+
+    // Validate the URI scheme of the upload directory.
+    $scheme = file_uri_scheme($values['directory']);
+    if (!$scheme || !in_array($scheme, $this->getSchemes())) {
+      form_set_error('directory', t('Please enter a valid scheme into the directory location.'));
+
+      // Return here so that attempts to create the directory below don't
+      // throw warnings.
+      return;
+    }
+
+    // Ensure that the upload directory exists.
+    if (!file_prepare_directory($values['directory'], FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS)) {
+      form_set_error('directory', t('The chosen directory does not exist and attempts to create it failed.'));
+    }
+  }
+
 }