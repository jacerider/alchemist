(function ($) {

Drupal.alchemist = Drupal.alchemist ? Drupal.alchemist : {};

Drupal.behaviors.alchemist = {
  alchemist_id: null,
  $link: null,
  $linkChild: null,
  linkActive: false,
  linkTimeout: null,
  // $panel: null,
  // $panelShadow: null,
  // $panelWrapper: null,
  // $panelContent: null,
  // $panelClose: null,

  attach: function (context, settings) {
    var self = this;
    $('body', context).once('alchemist').each(function(){
      // self.panelInit();
      self.editableLink(context, settings);
    });
    // Resize
    // Drupal.alchemist.modelPosition();

    // $('#st-open').once().click(function(event){
    //   event.stopPropagation();
    //   event.preventDefault();
    //   setTimeout( function() {
    //     $('#st-container').addClass('st-menu-open');
    //   }, 25 );
    //   $(document).click(function(){
    //     $('#st-container').removeClass('st-menu-open');
    //     $(document).unbind('click');
    //   });
    // });
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

  // panelInit: function(){
  //   var self = this;
  //   self.$panelShadow = $('<div id="alc-panel-shadow" />').appendTo($('body'));
  //   self.$panel = $('<div id="alc-panel" />').appendTo($('body'));
  //   self.$panelTitle = $('<div id="alc-panel-title" />').appendTo(self.$panel);
  //   self.$panelContent = $('<div id="alc-panel-content" />').appendTo(self.$panel);
  //   self.$panelClose = $('<a href="" id="alc-panel-close" />').appendTo(self.$panel);

  //   self.$panelClose.click(function(e){
  //     e.preventDefault();
  //     Drupal.alchemist.panelClose();
  //   });
  // }
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
  $('.alc-pusher').click(function(){
    $panel.removeClass('alc-region-open');
    $('.alc-pusher').unbind('click');
  });
}

// Drupal.alchemist.modelPosition = function(ajax, response, status){
//   var $panel = Drupal.behaviors.alchemist.$panel;
//   var $content = Drupal.behaviors.alchemist.$panelContent;

//   var height = $panel.outerHeight();
//   var width = $panel.outerWidth();
//   var winheight = $(window).height();
//   var winwidth = $(window).width();
//   var scrollTop = $(window).scrollTop();
//   var x = (winwidth / 2) - (width / 2);
//   var y = scrollTop + 30;

//   // if(height > winheight){
//   //   y = 0;
//   //   $content.css({height: winheight - 100 + 'px'});
//   // }

//   // if(width > winwidth){
//   //   x = 0;
//   //   $content.css({width: winwidth + 'px'});
//   // }

//   $panel.css({left: x + 'px',top: y + 'px'});
// }

Drupal.alchemist.panelClose = function(ajax, response, status){
  $('#alc-panel').removeClass('alc-region-open');
  $('.alc-pusher').unbind('click');

  // var $panel = Drupal.behaviors.alchemist.$panel;
  // var $panelShadow = Drupal.behaviors.alchemist.$panelShadow;
  // var $content = Drupal.behaviors.alchemist.$panelContent;
  // var $title = Drupal.behaviors.alchemist.$panelTitle;
  // $panel.removeClass('active');
  // $panelShadow.removeClass('active');
  // $panel.removeAttr('style');
  // $content.html('');
  // $title.html('');
}

// Add our commands to the Drupal commands collection.
Drupal.ajax.prototype.commands.alchemistPanel = Drupal.alchemist.panel;
Drupal.ajax.prototype.commands.alchemistPanelPosition = Drupal.alchemist.panelPosition;
Drupal.ajax.prototype.commands.alchemistPanelClose = Drupal.alchemist.panelClose;

})(jQuery);
