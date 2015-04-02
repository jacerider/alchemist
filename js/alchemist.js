(function ($) {

Drupal.alchemist = Drupal.alchemist ? Drupal.alchemist : {};

Drupal.behaviors.alchemist = {
  alchemist_id: null,
  $link: null,
  $linkChild: null,
  linkActive: false,
  linkTimeout: null,

  attach: function (context, settings) {
    var self = this;
    $('body', context).once('alchemist').each(function(){
      self.editableLink(context, settings);
    });
    $('.alc-panel-close', context).click(function(e){
      e.preventDefault();
      Drupal.alchemist.panelClose();
    });
  },

  editableLink: function(context, settings){
    var self = this, selectorElement, options, $wrapper, $toolbar, $link,
      alcClass, offset, fieldname, element_settings, base, count = 0;

    self.alchemist_id = settings.alchemist.alchemist_id;

    for (selectorElement in settings.alchemist.editable) {
      options = settings.alchemist.editable[selectorElement];
      $(selectorElement).each(function(){
        var $this = $(this);
        $this.addClass('alc-field-editable');

        offset = $this.offset();
        alcClass = options.dark ? ' dark' : '';

        $wrapper = $('<div class="alc-fe'+alcClass+'"></div>').appendTo($(this))
          .append('<div class="alc-fe-outline" />');

        $toolbar = $('<div class="alc-fe-toolbar"></div>').appendTo($wrapper)
          .addClass('tool-right')
          .append('<div class="arrow" />');

        if(offset.top <= 10 || offset.left <= 10){
          $toolbar.addClass('padding');
        }

        $link = $('<a class="alc-fe-link" href="#" />').appendTo($toolbar)
          .append('<i class="fa fa-edit"></i>' + options.label)
          .hover(function(){
            $(this).closest('.alc-fe').addClass('active');
          }, function(){
            $(this).closest('.alc-fe').removeClass('active');
          });
        // fieldname = $this.attr('data-alc-fieldname');
        // base = 'alc-' + count;
        element_settings = {
          url: 'http://' + window.location.hostname +  settings.basePath + settings.pathPrefix + 'alchemist/field/' + self.alchemist_id + '/' + options.field_name,
          event: 'click',
          progress: {
            type: 'throbber'
          }
        };
        Drupal.ajax[self.field_name] = new Drupal.ajax(self.field_name, $link, element_settings);
        count++;
      });
    }
  }
};

/**
 * AJAX command to place HTML within a BeautyTip.
 */
Drupal.alchemist.panel = function(ajax, response, status) {
  var $panel = $('#alc-panel');
  var $region = $('#alc-region-' + response.position);
  var $content = $('.alc-region-content', $region);
  var $inner = $('.alc-region-inner', $region);
  var $title = $('.alc-region-title', $region);
  // Hide anything not within alc-panel.
  $('body > div:not(#alc-panel)').addClass('alc-hide');
  $content.html(response.data);
  $title.html(response.title);
  $region.removeClass('alc-overflow')
  Drupal.attachBehaviors($content);
  $panel.removeClass (function (index, css) {
    return (css.match (/(^|\s)alc-effect\S+/g) || []).join(' ');
  }).addClass('alc-effect-' + response.position);
  setTimeout( function() {
    $panel.addClass('alc-region-open');
    // We do not always want the top bar to have an overflow of hidden. This can
    // hide UI elements.
    if(response.position = 'top' && $(window).height() > $inner.height()){
      $region.mCustomScrollbar("disable");
    }
    else{
      $region.mCustomScrollbar("update");
    }
  }, 25 );
  $('#alc-pusher').click(function(){
    $panel.removeClass('alc-region-open');
    $('#alc-pusher').unbind('click');
  });
}

Drupal.alchemist.panelClose = function(ajax, response, status){
  $('.alc-hide').removeClass('alc-hide');
  $('#alc-panel').removeClass('alc-region-open');
  $('.alc-pusher').unbind('click');
}

// Add our commands to the Drupal commands collection.
Drupal.ajax.prototype.commands.alchemistPanel = Drupal.alchemist.panel;
Drupal.ajax.prototype.commands.alchemistPanelClose = Drupal.alchemist.panelClose;

})(jQuery);
