(function ($, Drupal) {

Drupal.alchemist.field.abstract = function(options){
  var self = this;

  // Default settings
  self.settings = $.extend({}, {
    alchemistId: null,
    fieldId: null,
    fieldName: null,
    display: null,
    label: null,
    type: null
  }, options);

  // Get fields of same fieldname excluding self.
  this.getFields = function(fieldName) {
    var fields = self.getFieldsAll(self.settings.fieldName);
    var results = {};
    for (var x in fields) {
      var field = fields[x];
      if(field.fieldId != self.settings.fieldId){
        results[x] = field;
      }
    }
    return results;
  }

  // Get all fields of same fieldname.
  this.getFieldsAll = function(fieldname) {
    return Drupal.settings.alchemist.fields[self.settings.fieldName];
  }

  /**
   * Called when a field is ready for Alchemist editability.
   */
  this.render = function() {
    self.editInit();
  }

  /**
   * Basic editable functionality.
   */
  this.editInit = function() {
    $('#' + self.settings.fieldId).click(this.editClick.bind(this));
  }

  /**
   * Edit click callback.
   */
  this.editClick = function(e) {
    var self = this;

    if(!Drupal.alchemist.active){
      var url = Drupal.settings.basePath + 'alchemist/' + self.settings.alchemistId + '/' + self.settings.fieldId;

      Drupal.ajax['alchemist_edit'] = new Drupal.ajax(null, $('#' + self.settings.fieldId), {
        url: url,
        event: 'onload',
        keypress: false,
        prevent: false,
        success: function(response, status) {
          Drupal.ajax['alchemist_edit'].alchemistSuccess(response, status);
          self.editShow();
        }
      });
      Drupal.ajax['alchemist_edit'].alchemistRun();
    }
    else{
      if(Drupal.alchemist.active != this.settings.fieldId){
        alert('Please save current edit before starting a new one.');
      }
    }
  }

  /**
   * Called when edit interface is shown to user.
   */
  this.editShow = function(){
  }

}

Drupal.alchemist.field.abstract.prototype = {
    display: null,
    label: null,
    type: null
};

})(jQuery, Drupal);
