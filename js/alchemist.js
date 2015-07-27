(function ($, Drupal) {

Drupal.alchemist = Drupal.alchemist ? Drupal.alchemist : {$alchemist:$('#alchemist'),field:{},active:null};

Drupal.behaviors.alchemist = {

  attach: function(context, settings) {
    var self = this;
    if(settings.alchemist){
      if(!settings.alchemist.once){
        self.once(settings.alchemist, context);
      }
      self.repeat(settings.alchemist, context);
    }
  },

  once: function(alchemist, context) {
    var self = this, instances, fieldName, fieldId, options;
    alchemist.once = 1;

    // $('.alc-field').hover(function(e){
    //   $('body').addClass('alc-hover');
    // }, function(e){
    //   $('body').removeClass('alc-hover');
    // });

    for (fieldName in alchemist.fields) {
      instances = alchemist.fields[fieldName];
      for (fieldId in instances) {
        options = instances[fieldId];
        options.fieldId = fieldId;
        options.fieldName = fieldName;

        if(Drupal.alchemist.field[options.type]){
          options.handler = new Drupal.alchemist.field[options.type](options);
        }
        else{
          options.handler = new Drupal.alchemist.field.abstract(options);
        }
        // options.handler.prepare();
      }
    }
  },

  repeat: function(alchemist, context) {
    var self = this, instances, fieldId, options;
    for (fieldName in alchemist.fields) {
      instances = alchemist.fields[fieldName];
      for (fieldId in instances) {
        options = instances[fieldId];
        options.handler.prepare();
      }
    }
    // $('.alc-field').once('alc').each(function(){
    //   var $this = $(this);
    //   var $child = $this.children();
    //   $this.addClass($child.attr('class'));
    // });
  }
};

/**
 * Flag exo as being edited.
 */
Drupal.ajax.prototype.commands.alchemistEditingStart = function (ajax, response, status) {
  if(response.field_id){
    Drupal.alchemist.active = response.field_id;
    $('*[data-alc-id="' + response.field_id + '"]').addClass('active');
    $('body').addClass('alc-active').removeClass('alc-hover');
  }
};

/**
 * Flag exo as being edited.
 */
Drupal.ajax.prototype.commands.alchemistEditingStop = function (ajax, response, status) {
  Drupal.alchemist.active = null;
  $('.alc-field.active').removeClass('active');
  $('body').removeClass('alc-active');
};

/**
 * Command to insert new content into eXo.
 */
Drupal.ajax.prototype.commands.alchemistEdit = function (ajax, response, status) {
  console.log('ajax', ajax);
  console.log('response', response);
  console.log('status', status);
  // var settings = response.settings || ajax.settings || Drupal.settings;
  // $('#exo-content').exoFrame('paneHide').exoFrame('insertHtml', response.data);
};

/**
 * Add an extra function to the Drupal ajax object
 * which allows us to trigger an ajax response without
 * an element that triggers it.
 */
Drupal.ajax.prototype.alchemistRun = function() {
  var ajax = this;
  // Do not perform another ajax command if one is already in progress.
  if (ajax.ajaxing) {
    return false;
  }
  try {
    ajax.beforeSerialize(ajax.element, ajax.options);
    $.ajax(ajax.options);
  }
  catch (err) {
    alert('An error occurred while attempting to process ' + ajax.options.url);
    return false;
  }
  return false;
};

/**
 * Clone the success method so it is not overrided by configurable callbacks.
 */
Drupal.ajax.prototype.alchemistSuccess = Drupal.ajax.prototype.success;

})(jQuery, Drupal);
