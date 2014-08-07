(function ($) {

Drupal.behaviors.alchemistScrollbar = {
  attach: function (context) {
    $("#alc-panel .alc-region").once('alc').mCustomScrollbar({
        theme:"dark",
        scrollButtons:{
          enable:true
        }
    });
  }
};

})(jQuery);
