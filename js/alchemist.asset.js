(function ($) {

Drupal.alchemist = Drupal.alchemist ? Drupal.alchemist : {};

Drupal.behaviors.alchemist_asset = {
  attach: function (context, settings) {
    var self = this;
    $('.alchemist-asset-browser .asset-select', context).once().click( self.assetSelect );
  },

  assetSelect: function ( e ) {
    e.preventDefault();
    var self = this;
    var $this = $(this);
    var aid = parseInt($this.attr('data-id'));
    var editor = Drupal.alchemist.editor;
    var settings = Drupal.settings;
    var key = aid + '-0';
    if(settings.asset && settings.asset.render && settings.asset.render[key]){
      editor.insertHtml(settings.asset.render[key]);
    }
    else{
      alert('Asset could not be rendered.');
    }
    editor.focus();
    Drupal.alchemist.panelClose();
  }
};

Drupal.alchemist.insert = function(ajax, response, status){
  var markup = response.markup;
  var editor = Drupal.alchemist.editor;
  markup = markup.replace(/(\r\n|\n|\r)/gm,"")
  editor.insertHtml(markup);
}

Drupal.alchemist.asset = function(ajax, response, status){
  var aid = response.aid;
  var iid = response.iid ? response.iid : 0;
  var editor = Drupal.alchemist.editor;
  var settings = Drupal.settings;
  var key = aid + '-' + iid;
  if(settings.asset && settings.asset.render && settings.asset.render[key]){
    response.markup = settings.asset.render[key];
    Drupal.alchemist.insert(ajax, response, status);
  }
}

// Add our commands to the Drupal commands collection.
Drupal.ajax.prototype.commands.alchemistInsert = Drupal.alchemist.insert;
Drupal.ajax.prototype.commands.alchemistAsset = Drupal.alchemist.asset;

})(jQuery);
