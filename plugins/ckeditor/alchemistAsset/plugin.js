( function() {

  'use strict';

  if(!jQuery){
    return;
  }

  if(!Drupal && !Drupal.settings && !Drupal.settings.asset && !Drupal.settings.asset.fields){
    return;
  }

  /*****************************************************************************
  * Initial Setup
  *****************************************************************************/

  var alchemistAssets = function() {
    this.assetBar = assetBar;
    this.assetBarDimensions = false;
    this.editorPosition = false;
    this.assetBarContentSet = false;
    this.assetBarDisabled = false;
  };
  var is_locked = false;
  var is_visible = false;

  var alchemistAssetLinkTpl = CKEDITOR.addTemplate( 'alchemistAssetLink', '<span class="alchemist-asset-link"><a href="{url}" class="use-ajax"><i class="fa fa-{icon}"></i></a></span>' );

  // Create parent divs.
  var div = document.createElement('div');
  div.id = 'alchemist-assets';
  div.innerHTML = '<a id="alchemist-assets-toggle"><i class="fa fa-plus"></i></a><div id="alchemist-assets-content"></div>';
  jQuery('#alc-content').append(div);

  // Asset bar variables.
  var assetBar = CKEDITOR.document.getById('alchemist-assets');
  var assetBarToggle = CKEDITOR.document.getById('alchemist-assets-toggle');
  var assetBarContent = CKEDITOR.document.getById('alchemist-assets-content');

  assetBar.$.onmouseover=function(){
    is_locked = true;
  };
  assetBar.$.onmouseout=function(){
    is_locked = false;
  };
  assetBarToggle.$.onclick=function(event){
    if(assetBar.active){
      assetBarContentHide();
    }
    else{
      assetBarContentShow();
    }
    event.preventDefault ? event.preventDefault() : (event.returnValue=false);
  };

  var assetBarContentShow = function(){
    assetBar.active = true;
    jQuery(assetBar.$).removeClass('bar-hide').addClass('bar-show');
  }

  var assetBarContentHide = function(){
    assetBar.active = false;
    jQuery(assetBar.$).removeClass('bar-show').addClass('bar-hide');
  }

  CKEDITOR.plugins.add( 'alchemistAsset', {
    requires: "widget",
    init: function( editor ) {

      // editor.filter.allow( 'div[data-*,typeof,about](*); img[!src,alt]; iframe[!src,width,height,frameborder]; ul(*); li; style;', 'Asset' );
      // editor.filter.allow( 'div[data-*,typeof,about](*); img[!src,alt]; style; i(*);', 'Asset' );
      editor.filter.allow( 'div[data-*,typeof,about](*); img[!src,alt]; style; icon(*); a(*);', 'Asset' );

      editor.getAlchemistData = assets;

      editor.on( 'focus', function() {
        editor.alchemistAssets = new alchemistAssets();
        // Hide by default
        position(editor, 'hide');
        // Store active editor
        Drupal.alchemist.editor = editor;
      });

      editor.on('contentDom', function( event ) {

        editor.container.on('keyup', function( mouse_event ) {
          position(editor);
        });

        editor.container.on('mouseup', function( mouse_event ) {
          position(editor);
        });

        editor.container.on('blur', function( mouse_event ) {
          position(editor, 'hide');
        });

        editor.on('insertHtml', function( mouse_event ) {
          // Workaround for positioning
          jQuery(window).trigger('resize');
        });

        // Rebind Drupal behaviors
        // Drupal.attachBehaviors(jQuery(editor.config.field));
        jQuery(window).trigger('resize');
      });

      editor.widgets.add("asset", {
        // button: "Create a simple box",
        // allowedContent: "div[class,id]",
        // requiredContent: "div",
        upcast: function(element) {
          return element.name === "div" && element.hasClass("entity-asset");
        },
        edit: function(event) {
          var $wrapper, $element, aid, iid, url, base, element_settings;
          var asset_type, entity_type = editor.config.entity_type, field_name = editor.config.field_name, settings;
          // Save widget for use later.
          // Drupal.alchemist.asset = event.sender.wrapper;
          // Trigger edit dialog.
          $wrapper = jQuery(event.sender.wrapper.$);
          $element = jQuery('.entity-asset', $wrapper).first();
          aid = $element.attr('data-aid');
          iid = $element.attr('data-iid');
          asset_type = $element.attr('data-type');
          // url = Drupal.settings.basePath + 'alchemist/asset/' + aid + '/' + iid;
          url = '';
          if(Drupal.settings.asset.fields[entity_type] && Drupal.settings.asset.fields[entity_type][field_name] && Drupal.settings.asset.fields[entity_type][field_name][asset_type]){
            // If selected field has instance settings, we need to trigger the instance panel for updates.
            settings = Drupal.settings.asset.fields[entity_type][field_name][asset_type];
            // Show instance edit interface if has a public widget. If require
            // widget is set we should load asset interface as multiple assets
            // are not needed.
            if(settings.public && !settings.data.require_instance){
              url = Drupal.settings.basePath + 'alchemist/asset/' + aid + '/' + iid;
            }
            else{
              url = Drupal.settings.basePath + 'alchemist/asset/' + aid;
            }
          }
          if(url){
            base = 'alchemist';
            element_settings = {
              url: url,
              event: "onload",
              keypress: false,
              prevent: false,
              wrapper: base
            };
            Drupal.ajax[base] = new Drupal.ajax(base, $element, element_settings);
            return $element.trigger('onload').off('onload');
          }
          alert('There was an error with your request.');
        },
        // template: "<div class=\"entity-asset\">" + "<h2 class=\"exo-asset-title\">Title</h2>" + "<div class=\"exo-asset-content\"><p>Content...</p></div>" + "</div>"
      });

    }
  });

  function position( editor, op ) {
    var element, getElementPosition, getSelectionPosition, getAssetBarDimensions,
        getAssetBar, getEditorPosition, setAssetPosition, calculatePosition,
        showAssetBar, hideAssetBar, setAssetBarContent;

    // If bar is locked, just return.
    if(is_locked){
      return;
    }

    getAssetBar = function(){
      return editor.alchemistAssets.assetBar;
    }

    showAssetBar = function(){
      var assetBar = getAssetBar();
      setAssetBarContent();
      // Don't show if no assets are enabled.
      if(editor.alchemistAssets.assetBarDisabled){
        assetBar.$.className = 'hide';
        return;
      }
      assetBar.$.className = 'active in';
      is_visible = true;
    }

    hideAssetBar = function(){
      var assetBar = getAssetBar();
      // Don't hide if no assets are enabled.
      if(editor.alchemistAssets.assetBarDisabled) return;
      // Hide asset bar content
      assetBarContentHide();
      assetBar.$.className = 'active out';
      is_visible = false;
    }

    if(op == 'hide'){
      if(is_visible) hideAssetBar();
      return;
    }

    /**
     * Get position and dimensions of an element.
     */
    getElementPosition = function(e){
      var content = jQuery('#alc-content');
      var obj = e.$, xPosition = 0, yPosition = 0;
      var height = obj.offsetHeight, width = obj.offsetWidth;
      var offset = jQuery(obj).offset();
      var offsetTop = offset.top + content.scrollTop();
      var offsetLeft = offset.left + content.scrollLeft();
      return { left: offset.left, top: offsetTop, height: height, width: width};
    }

    /**
     * Get current selection dimensions and position.
     */
    getSelectionPosition = function(){
      return getElementPosition(element);
    }

    /**
     * Get current selection dimensions and position.
     */
    getEditorPosition = function(){
      if (! editor.alchemistAssets.editorPosition) {
        editor.alchemistAssets.editorPosition = getElementPosition(editor.container);
      }
      return editor.alchemistAssets.editorPosition;
    }

    /**
     * Returns the size of the floating toolbar
     */
    getAssetBarDimensions = function(reset) {
      if (! editor.alchemistAssets.assetBarDimensions || reset) {
        var obj = getAssetBar();
        editor.alchemistAssets.assetBarDimensions = {
          width: obj.$.offsetWidth,
          height: obj.$.offsetHeight
        };
      }
      return editor.alchemistAssets.assetBarDimensions;
    }

    calculatePosition = function(selectionPosition, assetBarDimensions, editorPosition){
      var toolpos = {
        x: (selectionPosition.left - assetBarDimensions.width) - 10,
        y: selectionPosition.top - (assetBarDimensions.height / 4) + 5,
      }

      if(toolpos.x < 0){
        toolpos.x = editorPosition.x;
        toolpos.y = selectionPosition.top - (assetBarDimensions.height + 10);
      }

      return toolpos;
    }

    setAssetPosition = function(){
      var assetBar = getAssetBar();
      var selectionPosition = getSelectionPosition();
      var assetBarDimensions = getAssetBarDimensions();
      var editorPosition = getEditorPosition();
      var position = calculatePosition(selectionPosition, assetBarDimensions, editorPosition);
      assetBar.setStyles({
        'left' : position.x + 'px',
        'top' : position.y + 'px'
      });
    }

    setAssetBarContent = function(){
      if (! editor.alchemistAssets.assetBarContentSet ) {
        var link, settings, url;
        editor.alchemistAssets.assetBarContentSet = true;
        assetBarContent.$.innerHTML = '';
        var asset_type, settings, link, entity_type = editor.config.entity_type, field_name = editor.config.field_name;
        if(Drupal.settings.asset.fields[entity_type] && Drupal.settings.asset.fields[entity_type][field_name]){
          for (asset_type in Drupal.settings.asset.fields[entity_type][field_name]) {
            settings = Drupal.settings.asset.fields[entity_type][field_name][asset_type];
            // Inject the space into the target.
            url = Drupal.settings.basePath + 'alchemist/browser/' + asset_type
            // Assets that do not allow multiple should go directly to creation
            if(settings.data.no_multiple){
              url = Drupal.settings.basePath + 'alchemist/asset/' + asset_type
            }
            link = assetBarContent.append( CKEDITOR.dom.element.createFromHtml( alchemistAssetLinkTpl.output( {
              icon: settings.data.icon,
              url: url,
            } ) ) );
          }
          Drupal.attachBehaviors(assetBarContent.$, Drupal.settings);
        }
        else{
          editor.alchemistAssets.assetBarDisabled = true;
        }
      }
    }

    var selection = editor.getSelection();
    if( !selection.getSelectedText() ){
      var path = editor.elementPath();
      if(path.block && path.block.getName() == 'p' && path.block.$.innerHTML == '<br>'){
        element = path.block;
        showAssetBar();
        setAssetPosition();
        return;
      }
    }
    hideAssetBar();
  }

  // Assets to tokens.
  function assets(){
    var data, aid, iid, $data, $asset;
    data = this.getData();
    // Strip out last hanging empty <p> tag.
    var regex = new RegExp('.*(<p>(\s|&nbsp;|<\/?\s?br\s?\/?>)*<\/?p>[\n\r\f])(?![\f\n\r])');
    data = data.replace(regex, '');
    var regex = new RegExp('^(<p>\[.*\](\s)*<\/p>)$');
    var asdf = data.replace(regex, '$1');
    console.log(asdf);
    // Replace tokens
    var $data = jQuery('<div></div>').append(data);
    jQuery('.entity-asset', $data).each(function(){
      $asset = jQuery(this);
      aid = $asset.attr('data-aid');
      iid = $asset.attr('data-iid');
      $asset.replaceWith('[asset-' + aid + '-' + iid + ']');
    });

    return $data.html();
  }

} )();
