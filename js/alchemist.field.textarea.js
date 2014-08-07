(function ($) {

Drupal.behaviors.alchemist_field_textarea = {
  editors: {},


  attach: function (context, settings) {
    var i, self, alchemist, options;
    if (!settings.alchemist && !settings.alchemist.textarea) {
      return;
    }
    self = this;
    alchemist = settings.alchemist;

    // This property tells CKEditor to not activate every element with contenteditable=true element.
    CKEDITOR.disableAutoInline = true;
    CKEDITOR.plugins.addExternal('alchemistPlaceholder', alchemist.path + '/plugins/ckeditor/alchemistPlaceholder/');
    CKEDITOR.plugins.addExternal('alchemistToolbar', alchemist.path + '/plugins/ckeditor/alchemistToolbar/');
    CKEDITOR.plugins.addExternal('alchemistAsset', alchemist.path + '/plugins/ckeditor/alchemistAsset/');
    CKEDITOR.config.extraPlugins = 'alchemistPlaceholder,alchemistToolbar,alchemistAsset';
    // CKEDITOR.config.extraAllowedContent = 'div(*)[*]; img(*)[*]; a(*)[*]; i(*); span[!id]; iframe(*)[*]';
    CKEDITOR.config.skin = 'moono-dark,' + alchemist.path + '/plugins/ckeditor/moono-dark/';

    for (i in alchemist.textarea) {
      if ( self.editors[i] ){
        return;
      }
      options = alchemist.textarea[i];
      options.toolbar = [["Format"], ["Bold", "Italic", "-", "Link", "Unlink"], ["JustifyLeft", "JustifyCenter", "JustifyRight", "JustifyBlock"]];
      // options.fillEmptyBlocks = false;
      // CKEDITOR.replace( i, {extraPlugins: 'floating-tools'});
      self.editors[i] = CKEDITOR.inline( document.getElementById( i ), options);

      self.editors[i].on('change', function() {
        $(this.config.field).val(this.getAlchemistData());
      });
    }
  }
};

})(jQuery);
