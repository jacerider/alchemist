(function ($, Drupal) {

Drupal.alchemist.field.abstract = function(options){
  var self = this;

  // Default settings
  self.options = $.extend({}, {
    alchemistId: null,
    fieldId: null,
    fieldName: null,
    label: null,
    type: null
  }, options);

  /**
   * Called when a field is ready for Alchemist editability.
   */
  this.prepare = function() {
    self.options.$element = $('#' + self.options.fieldId).next();
    console.log(self.options.$element);
    $(self.options.$element).once('alchemist')
      .addClass('alc-field')
      .attr('data-alc-id', self.options.fieldId)
      .attr('data-alc-label', self.options.label)
      .hover(self.editHover, self.editHoverOff)
      .click(self.editClick);
  }

  /**
   * Edit hover callback.
   */
  this.editHover = function(e) {
    if(!Drupal.alchemist.active){
      $('body').addClass('alc-hover');
    }
  }

  /**
   * Edit hover off callback.
   */
  this.editHoverOff = function(e) {
    $('body').removeClass('alc-hover');
  }

  /**
   * Edit click callback.
   */
  this.editClick = function(e) {

    if(!Drupal.alchemist.active){
      var url = Drupal.settings.basePath + 'alchemist/' + self.options.alchemistId + '/' + self.options.fieldId;

      Drupal.ajax['alchemist_edit'] = new Drupal.ajax(null, $('#' + self.options.fieldId), {
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
      if(Drupal.alchemist.active != self.options.fieldId){
        alert('Please save current edit before starting a new one.');
      }
    }
  }

  /**
   * Called when edit interface is shown to user.
   */
  this.editShow = function(){
  }

};

})(jQuery, Drupal);
