(function ($) {

Drupal.behaviors.alchemist_field_textfield = {
  editors: {},
  attach: function (context, settings) {
    var i, self, alchemist, options;
    if (!settings.alchemist || !settings.alchemist.textfield) {
      return;
    }
    self = this;
    alchemist = settings.alchemist;
    for (i in alchemist.textfield) {
      if ( self.editors[i] ){
        return;
      }
      options = alchemist.textfield[i];
      self.editors[i] = $('#' + i);
      self.editors[i].data('config', options);
      self.editors[i].each(function() {
        var value = $(this).text();
        $(this).text('temp');
        $(this).css({minHeight:$(this).height()});
        $(this).text(value);
      }).on('blur keyup paste input', function() {
        // Clear out empty content editable markup.
        if(!$(this).text()){
          $(this).text('');
        }
        options = self.editors[i].data('config');
        $(options.field).val($(this).text());
      });
    }
  }
};

})(jQuery);
