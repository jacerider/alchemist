(function ($) {

Drupal.behaviors.alchemistFieldsetSummaries = {
  attach: function (context) {
    $('fieldset.alchemist-node-settings-form', context).drupalSetSummary(function (context) {
      return Drupal.checkPlain($('.form-item-alchemist input:checked', context).next('label').text());
    });

    // Provide the summary for the node type form.
    $('fieldset.alchemist-node-type-settings-form', context).drupalSetSummary(function(context) {
      var vals = [];

      // Default alchemist setting.
      $("input[name^='alchemist_enable']:checked", context).parent().each(function() {
        vals.push(Drupal.checkPlain('Enabled'));
      });
      if (!$('#edit-alchemist-enable', context).is(':checked')) {
        vals.unshift(Drupal.t('Disabled'));
      }

      return Drupal.checkPlain(vals.join(', '));
    });
  }
};

})(jQuery);
