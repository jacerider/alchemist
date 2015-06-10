(function ($, Drupal) {

Drupal.alchemist.field.title = function(options){
  var self = this;
  var alchemistField = new Drupal.alchemist.field.abstract(options);

  /**
   * Called when edit interface is shown to user.
   */
  alchemistField.editShow = function(){
    $('#alc-field-title-editable').focus().blur(function(){
      $('#alc-field-title-textfield').val($(this).text());
    });
  }

  return alchemistField;
}
Drupal.alchemist.field.title.prototype = new Drupal.alchemist.field.abstract();

})(jQuery, Drupal);
