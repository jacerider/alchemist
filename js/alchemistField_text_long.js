(function ($, Drupal) {

Drupal.alchemist.field.text_long = function(options){
  var self = this;
  var alchemistField = new Drupal.alchemist.field.abstract(options);

  // alchemistField.render = function() {
  //   if(this.settings.instance && this.settings.instance.inline){
  //     CKEDITOR.disableAutoInline = true;
  //     alchemistField.editableInline();
  //   }
  //   else{
  //     alchemistField.editable();
  //   }
  // }

  /**
   * Called when edit interface is shown to user.
   */
  alchemistField.editShow = function() {
    var self = this;
    var element, ckeditorOptions = {};
    ckeditorOptions.startupFocus = true;
    ckeditorOptions.toolbar = [["Format"], ["Bold", "Italic", "-", "alchemistLink", "Unlink"], ['NumberedList','BulletedList','-','Outdent','Indent','-','Blockquote'], ["JustifyLeft", "JustifyCenter", "JustifyRight"]];
    element = document.getElementById( this.settings.fieldId + '-inline' );
    self.editor = CKEDITOR.inline(element, ckeditorOptions);
    self.editor.on('change', function(e) {
      var value = this.getData();
      $('#' + self.settings.fieldId + ' textarea').val(value);
    });
  }

  /**
   * Basic editable functionality.
   */
  alchemistField.editableInline = function() {
    var self = this;
    var element, ckeditorOptions = {};
    ckeditorOptions.toolbar = [["Format"], ["Bold", "Italic", "-", "alchemistLink", "Unlink"], ['NumberedList','BulletedList','-','Outdent','Indent','-','Blockquote'], ["JustifyLeft", "JustifyCenter", "JustifyRight"]];
    if(this.settings.parent == 'view'){
      if(this.settings.removeWrap){
        $('#' + this.settings.fieldId).html($('#' + this.settings.fieldId + '>*').html());
      }
      element = document.getElementById( this.settings.fieldId );
    }
    else{
      element = document.getElementById( this.settings.fieldId + '-inline' );
    }
    this.editor = CKEDITOR.inline(element, ckeditorOptions);
    this.editor.on('change', function(e) {
      var value = this.getData();

      console.log('#' + this.settings.fieldId + ' textarea');
    });

    this.editor.on('focus', function(e){
      // Drupal.alchemist.active = self.settings.fieldId;
    });

    this.editor.on('blur', function(e){
      // if(Drupal.alchemist.active == self.settings.fieldId){
      //   var value = this.getData();
      //   var fields = self.getFields();
      //   for (var x in fields) {
      //     var field = fields[x];
      //     field.handler.editorSetData(value);
      //   }
      // }
    });
  }

  /**
   * Update editor
   */
  alchemistField.editorSetData = function(data){
    this.editor.setData(data, {asdf:true});
  }

  /**
   * Update editor
   */
  alchemistField.editorGetData = function(){
    return this.editor.getData();
  }

  return alchemistField;
}
Drupal.alchemist.field.text_long.prototype = new Drupal.alchemist.field.abstract();


Drupal.alchemist.field.text_with_summary = function(options){
  var self = this;
  var alchemistField = new Drupal.alchemist.field.text_long(options);
  // alchemistField.sexy = function() {
  //   console.log('yes, you are a sexy text_with_summary field');
  // }
  return alchemistField;
}

})(jQuery, Drupal);
